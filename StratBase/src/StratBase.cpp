#include "StratBase.h"
#include "CedarHelper.h"
#include "EnumStringMap.h"

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

int StratBase::run() {
  onCreate();

  switch (mode) {
    case BACKTEST: {
      //setup backtester
      backtester.registerCallback(std::bind(&StratBase::onMsgWrapper,
          this, std::placeholders::_1));
      backtester.run();
      LOG(INFO) << "run complete";
      break;
    }

    case LIVETEST:
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
