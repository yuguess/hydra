#include "BackTester.h"

std::string WindDataType[WindTypeNum] = {"MD", "Inx", "Trns"};

MdAdapter* BackTester::md = new WindMdAdapter();
ev_timer BackTester::timeout_watcher;
ProtoBufMsgHub Tester::msgHub;
int BackTester::preTick;
int BackTester::nowTick;
int BackTester::m_rate = 1;

void Tester::spreadCedarMD() {
}

BackTester::BackTester() {
  CedarJsonConfig::getInstance().getStringByPath("BackTesterCfg.startDate", m_startDate);   
  CedarJsonConfig::getInstance().getStringByPath("BackTesterCfg.endDate", m_endDate);   
  CedarJsonConfig::getInstance().getIntByPath("BackTesterCfg.dayLength", m_dayLength);

  CedarJsonConfig::getInstance().getIntByPath("BackTesterCfg.rate", m_rate);
  CedarJsonConfig::getInstance().getStringByPath("BackTesterCfg.dir", m_dir);

  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(std::bind(&BackTester::onMsg, this, std::placeholders::_1));
}

BackTester::BackTester(std::string startDate, std::string endDate, int dayLength, int rate, std::vector<std::string> symbols) : m_startDate(startDate), m_endDate(endDate), m_dayLength(dayLength) {
  if(startDate.empty())
    CedarJsonConfig::getInstance().getStringByPath("BackTesterCfg.startDate", m_startDate);   
  if(endDate.empty())
    CedarJsonConfig::getInstance().getStringByPath("BackTesterCfg.endDate", m_endDate);   
  if(dayLength == 0)
    CedarJsonConfig::getInstance().getIntByPath("BackTesterCfg.dayLength", m_dayLength);

  CedarJsonConfig::getInstance().getIntByPath("BackTesterCfg.rate", m_rate);
  CedarJsonConfig::getInstance().getStringByPath("BackTesterCfg.dir", m_dir);
  /* CedarJsonConfig::getInstance().getStringArrayWithTag(m_symbols, "BackTesterCfg.symbols"); */

  /* for (auto symbol : symbols) */
  /*   m_symbols.push_back(symbol); */

  /* /1* getFilesToBufQ(); *1/ */

  /* std::string timeInterval; */
  /* CedarJsonConfig::getInstance().getStringByPath("BackTesterCfg.timeInterval", timeInterval); */
  /* setTimeInterval(std::atoi(timeInterval.c_str())); */
  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(std::bind(&BackTester::onMsg, this, std::placeholders::_1));
}

BackTester::~BackTester() { delete md; }

void BackTester::updateMD() {
  msg = md->getCedarMsg();
}

void BackTester::setTimeInterval(int usec) {
  const int usPerS = 1000000;
  int sec = 0;
  if(usec >= usPerS) {
    sec = usec / usPerS;
    usec = usec % usPerS;
  }
  t.tv_sec = sec;
  t.tv_usec = usec;
}

void wrapToMktUpdt(MarketUpdate &mktUpdt, Json::Value v) {
  std::string windCode = v.getMemberNames()[0];
  const char* exchange = windCode.c_str() + 7;

  mktUpdt.set_symbol(windCode);
  mktUpdt.set_code(v[windCode]["szCode"].asString());
  mktUpdt.set_exchange(exchange);
  mktUpdt.set_status(v[windCode]["nStatus"].asInt());
  mktUpdt.set_pre_close(v[windCode]["nPreClose"].asInt());
  mktUpdt.set_open_price(v[windCode]["nOpen"].asDouble());
  mktUpdt.set_highest_price(v[windCode]["nHigh"].asDouble());
  mktUpdt.set_lowest_price(v[windCode]["nLow"].asDouble());
  mktUpdt.set_high_limit_price(v[windCode]["nHighLimited"].asDouble());
  mktUpdt.set_low_limit_price(v[windCode]["nLowLimited"].asDouble());
  mktUpdt.set_latest_price(v[windCode]["nMatch"].asDouble());
  mktUpdt.set_num_trades(v[windCode]["nNumTrades"].asInt());
  mktUpdt.set_volume(v[windCode]["iVolume"].asInt64());
  mktUpdt.set_total_bid_vol(v[windCode]["nTotalBidVol"].asInt64());
  mktUpdt.set_total_ask_vol(v[windCode]["nTotalAskVol"].asInt64());
  mktUpdt.set_weighted_avg_bid_price(v[windCode]["nWeightedAvgBidPrice"].asInt());
  mktUpdt.set_weighted_avg_ask_price(v[windCode]["nWeightedAvgAskPrice"].asInt());
  mktUpdt.set_iopv(v[windCode]["nIOPV"].asInt());
  mktUpdt.set_yield_to_maturity(v[windCode]["nYieldToMaturity"].asInt());
  mktUpdt.set_action_day(v[windCode]["nActionDay"].asInt());
  mktUpdt.set_exchange_timestamp(v[windCode]["nTime"].asString());

  for (int j = 0; j < priceLevel; j++) {
    mktUpdt.add_bid_price(stoi(v[windCode]["nBidPrice"][j].asString()));
    mktUpdt.add_bid_volume(stoi(v[windCode]["nBidVol"][j].asString()));
    mktUpdt.add_ask_price(stoi(v[windCode]["nAskPrice"][j].asString()));
    mktUpdt.add_ask_volume(stoi(v[windCode]["nAskVol"][j].asString()));
  }
}

double calTDiff(int now, int pre) {
  double diff = 0;
  diff += now % 1000 - pre % 1000;
  diff /= 1000.0;
  now /= 1000;
  pre /= 1000;

  diff += now % 100 - pre % 100;
  now /= 100;
  pre /= 100;

  diff += (now % 100 - pre % 100) * 60;
  now /= 100;
  pre /= 100;

  diff += (now - pre) * 3600;
  /* LOG(INFO) << diff; */
  return diff;
}

static void timeout_cb(EV_P_ ev_timer *w, int revents) { 
  Json::Value v;
  
  std::queue<Json::Value> *qt = &(BackTester::md->bufQueues[BackTester::md->bufpq.top().index]);

  std::string windCode = (qt->front()).getMemberNames()[0];
  BackTester::nowTick = (qt->front())[windCode]["nTime"].asInt();
  /* LOG(INFO) << windCode << " " << (qt->front())[windCode]["nActionDay"] << " " << BackTester::nowTick; */

  pqElem qe;
  qe.index = BackTester::md->bufpq.top().index;

  MarketUpdate mktUpdt;
  wrapToMktUpdt(mktUpdt, qt->front());

  std::string code = (qt->front()).getMemberNames()[0];
  std::string res = 
    ProtoBufHelper::wrapMsg<MarketUpdate>(TYPE_MARKETUPDATE, mktUpdt);
  BackTester::msgHub.boardcastMsg(code, res);

  qt->pop();
  BackTester::md->bufpq.pop();

  if(qt->empty()) {
    LOG(INFO) << "queue " << BackTester::md->bufpq.top().index << " is empty now";
  } else {
    v = qt->front();
    qe.dd = v[v.getMemberNames()[0]]["nActionDay"].asInt();
    qe.td = v[v.getMemberNames()[0]]["nTime"].asInt();

    BackTester::md->bufpq.push(qe);
  }

  if(BackTester::md->bufpq.empty()) {
    LOG(INFO) << "buf priority queue is empty now";
  } else {
    /* double interval = calTDiff(BackTester::nowTick, BackTester::preTick); */
    double interval = 0.01;
    interval = interval / BackTester::m_rate;
    BackTester::preTick = BackTester::nowTick;
    ev_timer_stop(loop, &(BackTester::timeout_watcher));
    ev_timer_set(&(BackTester::timeout_watcher), interval, 0.);
    ev_timer_start(loop, &(BackTester::timeout_watcher));
  }
}

void BackTester::run() {
  getFilesToBufQ();

  for(int i = 0; i < md->bufQueues.size(); i++) {
      pqElem qe;
      std::queue<Json::Value> q = md->bufQueues[i];
      Json::Value v = q.front();
      qe.dd = v[v.getMemberNames()[0]]["nActionDay"].asInt();
      qe.td = v[v.getMemberNames()[0]]["nTime"].asInt();
      qe.index = i;
      md->bufpq.push(qe);
  }

  preTick = md->bufpq.top().td;
  ev_timer_init(&timeout_watcher, timeout_cb, 0.1, 0.);
  ev_timer_start(loop, &timeout_watcher);
  ev_run(loop, 0);
}

void BackTester::newTick(int sock, int event, void* arg) {
  BackTester* bt = static_cast<BackTester*>(arg);
  bt->newTickAction();
  bt->md->nextTick();
}

void BackTester::newTickAction() {
  updateMD();
  spreadCedarMD();
}

int BackTester::getFilesToBufQ() {
  struct dirent **filelist;
  int indexOfDir = 0;

  if((indexOfDir = scandir(m_dir.c_str(), &filelist, 0, alphasort)) < 0)
    LOG(FATAL) << "Error scan" << m_dir << ", pls check your path";

  while (indexOfDir >= 3 && strncmp(filelist[--indexOfDir]->d_name, m_endDate.c_str(), 8) >= 0) {
    free(filelist[indexOfDir]);
  }

  for (int i = 2; i <= indexOfDir && m_dayLength != 0; i++) {
    if(strncmp(filelist[i]->d_name, m_startDate.c_str(), 8) < 0)
      continue;
    LOG(INFO) << "d_name: " << filelist[i]->d_name;
    for (int j = 0; j < WindTypeNum; j++) {
      LOG(INFO) << "WindTypeNum j: " << j;
      for (int k = 0; k < m_symbols.size(); k++) {
        std::string filename = m_dir + "/" + filelist[i]->d_name + "/" + m_symbols[k] + "/" + WindDataType[j]; 
        if (std::ifstream(filename.c_str())) {
          LOG(INFO) << "file loading: " << filename;
          md->loadFile(filename, j);
        }
      }
    }
    m_dayLength--;
    free(filelist[i]);
  }

  free(filelist);
  fileIter = files.rbegin();
}

int BackTester::onMsg(MessageBase msg) {
  LOG(INFO) << "BackTester onMsg";
  if(msg.type() == TYPE_DATAREQUEST) {
    m_symbols.clear();
    md->bufpq = std::priority_queue<pqElem, std::vector<pqElem>, pqComp>();

    DataRequest dtRqst = ProtoBufHelper::unwrapMsg<DataRequest>(msg);

    for(int i = 0; i < dtRqst.code_size(); i++) {
      std::string str = dtRqst.code(i) + ".";
      m_symbols.push_back(str + dtRqst.exchange());
    }

    BackTester::run();
  } else
    LOG(WARNING) << "Recv invalid msg: " << msg.type();

  return 0;
}
