#include "LtsMarketUpdate.h"
#include "EnumStringMap.h"

LtsMarketUpdate::LtsMarketUpdate() :
  pMdFrontMdApi(NULL) {

  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(std::bind(&LtsMarketUpdate::onMsg,
        this, std::placeholders::_1));
}

LtsMarketUpdate::~LtsMarketUpdate() {
  pMdFrontMdApi->Release();
}

int LtsMarketUpdate::onMsg(MessageBase msg) {
  switch (msg.type()) {
    case TYPE_DATAREQUEST: {
      DataRequest dataRequest = ProtoBufHelper::unwrapMsg<DataRequest>(msg);
      onMDReq(dataRequest);
      break;
    }

    default:
      LOG(WARNING) << "recv invalid msg type " << msg.type();
      LOG(INFO) << msg.DebugString();
  }

  return 0;
}

bool LtsMarketUpdate::onMDReq(DataRequest &mdReq) {
  LOG(INFO) << "recv data request " << mdReq.DebugString();

  std::string reqStr = mdReq.code() + "." +
    EnumToString::toString(mdReq.exchange());

  LOG(INFO) << "try to subscribe req str " << reqStr;

  if (registerTickers.find(reqStr) != registerTickers.end()) {
    LOG(INFO) << "already subscribe " << reqStr;
    return false;
  }

  if (mdReq.exchange() == SHSE) {
    LOG(INFO) << "subscribe SH stock market data";
    registerTickers[reqStr] = mdReq.code();
    char *str = const_cast<char*>(registerTickers[reqStr].c_str());
    pMdFrontMdApi->SubscribeMarketData(&str, 1, const_cast<char*>("SSE"));
  } else if (mdReq.exchange() == SZSE) {
    LOG(INFO) << "subscribe SZ stock market data";
    registerTickers[reqStr] = mdReq.code();
    char *str = const_cast<char*>(registerTickers[reqStr].c_str());
    pMdFrontMdApi->SubscribeMarketData(&str, 1, const_cast<char*>("SZE"));
  }

  return true;
}

void LtsMarketUpdate::OnFrontConnected() {
  CSecurityFtdcReqUserLoginField reqUserLogin;
  memset(&reqUserLogin, 0, sizeof(reqUserLogin));

  std::string broker, user, pass;
  CedarJsonConfig::getInstance().getStringByPath("LTS.BrokerID", broker);
  CedarJsonConfig::getInstance().getStringByPath("LTS.UserID", user);
  CedarJsonConfig::getInstance().getStringByPath("LTS.Password", pass);

  LOG(INFO) << "BrokerID " << broker;
  LOG(INFO) << "UserId " << user;
  LOG(INFO) << "Passwd " << pass;

  strcpy(reqUserLogin.BrokerID, broker.c_str());
  strcpy(reqUserLogin.UserID, user.c_str());
  strcpy(reqUserLogin.Password, pass.c_str());

  if (pMdFrontMdApi->ReqUserLogin(&reqUserLogin, 1) == 0) {
    LOG(INFO) << "login req sent successfully";
  } else {
    LOG(ERROR) << "login req failed";
  }
}

void LtsMarketUpdate::OnFrontDisconnected(int nReason) {
  LOG(INFO) << "front disconnected";
  exit(0);
}

void LtsMarketUpdate::OnRspUserLogin(
  CSecurityFtdcRspUserLoginField *pRspUserLogin,
  CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

  if (pRspInfo->ErrorID != 0) {
    LOG(ERROR) << ">>>>>>> Chinese Equity MarketData API fail";
    LOG(ERROR) << "Failed to login, errorcode:" << pRspInfo->ErrorID
      << ", errormsg:" << pRspInfo->ErrorMsg << " requestid:" << nRequestID
      << ", chain:" << bIsLast;
  } else {
    LOG(INFO) << ">>>>>>> Chinese Equity MarketData API success";
  }
}

void LtsMarketUpdate::OnRspSubMarketData(
  CSecurityFtdcSpecificInstrumentField *pSpecificInstrument,
  CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

  LOG(INFO) << "subscribe status " << pRspInfo->ErrorID
    << " " << pRspInfo->ErrorMsg;
  LOG(INFO) << "Successfully subscribe " << pSpecificInstrument->InstrumentID
    << " " << pSpecificInstrument->ExchangeID;
}

void LtsMarketUpdate::OnRtnDepthMarketData(
  CSecurityFtdcDepthMarketDataField *pMD) {
  std::string chan;

  //hard code here
  LOG(INFO) << pMD->InstrumentID << " " << pMD->ExchangeID;
  if (strcmp(pMD->ExchangeID, "SSE") == 0) {
    chan = std::string(pMD->InstrumentID) + ".SHSE";
  } else {
    chan = std::string(pMD->InstrumentID) + ".SZSE";
  }

  MarketUpdate md;
  ltsMDToCedarMD(pMD, md);

  std::string res = ProtoBufHelper::wrapMsg<MarketUpdate>(
    TYPE_MARKETUPDATE, md);

  msgHub.boardcastMsg(chan, res);
}

bool LtsMarketUpdate::ltsMDToCedarMD(
  CSecurityFtdcDepthMarketDataField *pMD, MarketUpdate &md) {

  //printMDStruct(pMD);

  md.set_trading_day(pMD->TradingDay);
  md.set_code(pMD->InstrumentID);
  md.set_last_price(pMD->LastPrice);
  md.set_pre_close_price(pMD->PreClosePrice);
  md.set_open_price(pMD->OpenPrice);
  md.set_highest_price(pMD->HighestPrice);
  md.set_lowest_price(pMD->LowestPrice);
  md.set_volume(pMD->Volume);
  md.set_turnover(pMD->Turnover);
  md.set_high_limit_price(pMD->UpperLimitPrice);
  md.set_low_limit_price(pMD->LowerLimitPrice);

  if (strcmp(pMD->ExchangeID, "SSE") == 0) {
    md.set_exchange(SHSE);
  } else {
    md.set_exchange(SZSE);
  }

  md.add_bid_price(pMD->BidPrice1);
  md.add_bid_price(pMD->BidPrice2);
  md.add_bid_price(pMD->BidPrice3);
  md.add_bid_price(pMD->BidPrice4);
  md.add_bid_price(pMD->BidPrice5);

  md.add_bid_volume(pMD->BidVolume1 / 100);
  md.add_bid_volume(pMD->BidVolume2 / 100);
  md.add_bid_volume(pMD->BidVolume3 / 100);
  md.add_bid_volume(pMD->BidVolume4 / 100);
  md.add_bid_volume(pMD->BidVolume5 / 100);

  md.add_ask_price(pMD->AskPrice1);
  md.add_ask_price(pMD->AskPrice2);
  md.add_ask_price(pMD->AskPrice3);
  md.add_ask_price(pMD->AskPrice4);
  md.add_ask_price(pMD->AskPrice5);

  md.add_ask_volume(pMD->AskVolume1 / 100);
  md.add_ask_volume(pMD->AskVolume2 / 100);
  md.add_ask_volume(pMD->AskVolume3 / 100);
  md.add_ask_volume(pMD->AskVolume4 / 100);
  md.add_ask_volume(pMD->AskVolume5 / 100);

  return true;
}

bool LtsMarketUpdate::printMDStruct(CSecurityFtdcDepthMarketDataField *pMD) {
  LOG(INFO) << "TradingDay " << pMD->TradingDay;
  LOG(INFO) << "InstrumentID " << pMD->InstrumentID;
  LOG(INFO) << "ExchangeID " << pMD->ExchangeID;
  LOG(INFO) << "ExchangeInstID " << pMD->ExchangeInstID;
  LOG(INFO) << "LastPrice " << pMD->LastPrice;
  LOG(INFO) << "PreSettlementPrice " << pMD->PreSettlementPrice;
  LOG(INFO) << "PreClosePrice " << pMD->PreClosePrice;
  LOG(INFO) << "PreOpenInterest " << pMD->PreOpenInterest;
  LOG(INFO) << "OpenPrice " << pMD->OpenPrice;
  LOG(INFO) << "HighestPrice " << pMD->HighestPrice;
  LOG(INFO) << "LowestPrice " << pMD->LowestPrice;
  LOG(INFO) << "Volume " << pMD->Volume;
  LOG(INFO) << "Turnover " << pMD->Turnover;
  LOG(INFO) << "OpenInterest "<< pMD->OpenInterest;
  LOG(INFO) << "ClosePrice " << pMD->ClosePrice;
  LOG(INFO) << "SettlementPrice " <<pMD->SettlementPrice;
  LOG(INFO) << "UpperLimitPrice " <<pMD->UpperLimitPrice;
  LOG(INFO) << "LowerLimitPrice " <<pMD->LowerLimitPrice;
  LOG(INFO) << "PreDelta " << pMD->PreDelta;
  LOG(INFO) << "CurrDelta " << pMD->CurrDelta;
  LOG(INFO) << "UpdateTime " << pMD->UpdateTime;
  LOG(INFO) << "UpdateMillisec " << pMD->UpdateMillisec;

  LOG(INFO) << "BidPrice1 " << pMD->BidPrice1;
  LOG(INFO) << "BidVolume1 " << pMD->BidVolume1;
  LOG(INFO) << "AskPrice1 " << pMD->AskPrice1;
  LOG(INFO) << "AskVolume1 " << pMD->AskVolume1;
  LOG(INFO) << "BidPrice2 " << pMD->BidPrice2;
  LOG(INFO) << "BidVolume2 " << pMD->BidVolume2;
  LOG(INFO) << "AskPrice2 " << pMD->AskPrice2;
  LOG(INFO) << "AskVolume2 " << pMD->AskVolume2;
  LOG(INFO) << "BidPrice5 " << pMD->BidPrice5;
  LOG(INFO) << "BidVolume5 " << pMD->BidVolume5;
  LOG(INFO) << "AskPrice5 " << pMD->AskPrice5;
  LOG(INFO) << "AskVolume5 " << pMD->AskVolume5;
  LOG(INFO) << "AveragePrice " << pMD->AveragePrice;
  LOG(INFO) << "ActionDay " << pMD->ActionDay;

  return true;
}

void LtsMarketUpdate::run() {
  std::vector<std::string> fronts;
  CedarJsonConfig::getInstance().getStringArrayWithTag(fronts, "LTS.Front");

  pMdFrontMdApi = CSecurityFtdcMdApi::CreateFtdcMdApi("");
  pMdFrontMdApi->RegisterSpi(this);
  pMdFrontMdApi->RegisterFront(const_cast<char*>(fronts[0].c_str()));
  pMdFrontMdApi->Init();

  LOG(INFO) << "front server " << fronts[0];
}
