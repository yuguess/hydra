#include "StratBase.h"
#include "CedarHelper.h"
#include "EnumStringMap.h"
#include "JsonHelper.h"

StratBase::StratBase(): orderAgent(backtester, msgHub) {
  std::string modeStr;
  //Backtest, Livetest, LiveTrading
  CedarJsonConfig::getInstance().getStringByPath("Strategy.Mode", modeStr);
  mode = StringToEnum::toStrategyMode(modeStr);

  if (mode == LIVETEST || mode == LIVE_TRADING) {
    CedarJsonConfig::getInstance().getStringByPath(
      "StockDataServer.serverAddr", stkDataAddr);
    CedarJsonConfig::getInstance().getStringByPath(
      "StockDataServer.boardcastAddr", stkBoardcastAddr);
    CedarJsonConfig::getInstance().getStringByPath(
      "FuturesDataServer.serverAddr", futuresDataAddr);
    CedarJsonConfig::getInstance().getStringByPath(
      "FuturesDataServer.boardcastAddr", futuresBoardcastAddr);

    CedarJsonConfig::getInstance().getStringArrayWithTag(tickers,
      "Livetest.DataRequest", "code");
    CedarJsonConfig::getInstance().getStringArrayWithTag(exchanges,
      "Livetest.DataRequest", "exchange");
  }
}

bool StratBase::getBacktestRangeDateStr(int replayLength,
  std::string &rangeBeg, std::string &rangeEnd) {
  boost::posix_time::ptime ptimeNow = CedarTimeHelper::getCurPTime();

  boost::posix_time::ptime ptimeEnd = ptimeNow -
    boost::gregorian::days(1);
  boost::posix_time::ptime ptimeBeg= ptimeNow -
    boost::gregorian::days(1 + replayLength);

  rangeBeg = CedarTimeHelper::ptimeToStr("%Y%m%d", ptimeBeg);
  rangeEnd = CedarTimeHelper::ptimeToStr("%Y%m%d", ptimeEnd);
  return true;
}

bool StratBase::setupLiveTestConfig(Json::Value &livetestJsonConf) {
  CedarJsonConfig::getInstance().getJsonValueByPath("Livetest.Setup",
    livetestJsonConf);

  int replayLength = livetestJsonConf["ReplayLength"].asInt();
  std::string rangeBeg, rangeEnd;

  getBacktestRangeDateStr(replayLength, rangeBeg, rangeEnd);

  LOG(INFO) << "Replay " << rangeBeg << " -- " << rangeEnd;

  std::vector<std::string> streams;
  JsonHelper::getStringArrayWithTag(livetestJsonConf, "Streams", "", streams);
  for (unsigned i = 0; i < streams.size(); i++) {
    livetestJsonConf[streams[i]]["Range"]["Start"] = rangeBeg;
    livetestJsonConf[streams[i]]["Range"]["End"] = rangeEnd;
  }

  return true;
}

int StratBase::run() {
  onCreate();

  switch (mode) {
    case BACKTEST: {
      Json::Value btJsonConf;
      CedarJsonConfig::getInstance().getJsonValueByPath("Backtest",
        btJsonConf);
      backtester.registerCallback(std::bind(&StratBase::onMsgWrapper,
          this, std::placeholders::_1));
      backtester.run(btJsonConf);
      LOG(INFO) << "backtester replay complete";
      break;
    }

    case LIVETEST: {
      if (CedarJsonConfig::getInstance().hasMember("Livetest.Setup")) {
        mode = BACKTEST;

        Json::Value livetestJsonConf;
        setupLiveTestConfig(livetestJsonConf);
        backtester.registerCallback(std::bind(&StratBase::onMsgWrapper,
          this, std::placeholders::_1));
        backtester.run(livetestJsonConf);
        LOG(INFO) << "Livetest replay complete";

        mode = LIVETEST;
      }
    }

    case LIVE_TRADING: {
      ProtoBufHelper::setupProtoBufMsgHub(msgHub);
      msgHub.registerCallback(std::bind(&StratBase::onMsgWrapper,
            this, std::placeholders::_1));

      //send data request
      for (unsigned i = 0; i < tickers.size(); i++) {
        subscribeTicker(tickers[i], exchanges[i]);
      }

      CedarHelper::blockSignalAndSuspend();
      break;
    }

    default:
      LOG(FATAL) << "Invalid mode " << mode << "check your config please";
  }

  onExit();
  return 0;
}

int StratBase::onMsgWrapper(MessageBase msgBase) {
  onMsg(msgBase);
  return 0;
}

int StratBase::subscribeTicker(std::string code, std::string xchg) {
  ExchangeType xchgType = StringToEnum::toExchangeType(xchg);
  DataRequest mdReq;
  mdReq.set_code(code);
  mdReq.set_exchange(xchgType);

  std::string targetAddr;
  std::string targetBoardcastAddr;

  if (xchgType == SHSE || xchgType == SZSE) {
    targetAddr = stkDataAddr;
    targetBoardcastAddr = stkBoardcastAddr;
  } else {
    targetAddr = futuresDataAddr;
    targetBoardcastAddr = futuresBoardcastAddr;
  }

  msgHub.pushMsg(targetAddr,
    ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST, mdReq));

  std::string chan = code + "." + xchg;

  LOG(INFO) << "send data request to [" << targetAddr << "]";
  LOG(INFO) << mdReq.DebugString();

  msgHub.addSubscription(targetBoardcastAddr, chan);

  return 0;
}
