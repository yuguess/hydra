#include "StratBase.h"
#include "CedarHelper.h"
#include "EnumStringMap.h"

int StratBase::run() {

  onCreate();

  std::string modeStr;
  //Backtest, Livetest, LiveTrading
  CedarJsonConfig::getInstance().getStringByPath("Strategy.Mode", modeStr);
  mode = StringToEnum::toStrategyMode(modeStr);

  switch (mode) {
    case BACKTEST: {
      //setup backtester
      backtester.registerCallback(std::bind(&StratBase::onMsgWrapper,
          this, std::placeholders::_1));
      backtester.run();
      break;
    }

    case LIVETEST:
    case LIVE_TRADING: {
      ProtoBufHelper::setupProtoBufMsgHub(msgHub);
      msgHub.registerCallback(std::bind(&StratBase::onMsgWrapper,
            this, std::placeholders::_1));

      CedarHelper::blockSignalAndSuspend();
    }

    default:
      LOG(FATAL) << "Invalid mode " << mode << "check your config please";
  }

  return 0;
}

int StratBase::onMsgWrapper(MessageBase msgBase) {
  onMsg(msgBase);
  return 0;
}
