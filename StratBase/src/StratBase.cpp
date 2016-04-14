#include "StratBase.h"

int StratBase::run() {
  //Backtest, Livetest, LiveTrading
  CedarJsonConfig::getInstance().getStringByPath("StratBase.Mode", mode);
  if (mode == "Backtest") {
    //setup backtester
    backtester.registerCallback(std::bind(&StratBase::onMsgWrapper,
          this, std::placeholders::_1));
    backtester.run();

  } else if (mode == "LiveTrading") {
    //setup msghub and blockon
    ProtoBufHelper::setupProtoBufMsgHub(msgHub);
    msgHub.registerCallback(std::bind(&StratBase::onMsgWrapper,
          this, std::placeholders::_1));

  } else {
    LOG(FATAL) << "Invalid mode " << mode << "check your config please";
  }

  return 0;
}

int StratBase::onMsgWrapper(MessageBase msgBase) {
  onMsg(msgBase);
}

template<typename T>
int StratBase::sendRequest(MsgType type, T &obj) {

  if (mode == "Backtest" || mode == "LiveTest") {
    //goes to backtester
    //backtester.sendRequest();
  } else if (mode == "LiveTrading") {
    //send through orderAgent
  } else {
    LOG(FATAL) << "Invalid mode " << mode << "check your config please";
  }

  return 0;
}
