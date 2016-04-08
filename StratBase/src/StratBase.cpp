#include "StratBase.h"

int StratBase::run() {
  //Backtest, Livetest, LiveTrading 
  CedarJsonConfig::getInstance().getStringByPath("StratBase.Mode", mode);
  if (mode == "Backtest") {
    //setup backtester
    //run backtester
    backtester.run();
  } else if (mode == "LiveTrading") {
    //setup msghub
    //blockon
  } else {
    LOG(FATAL) << "Invalid mode " << mode << "check your config please"; 
  }

  return 0;
}

template<typename T>
int StratBase::sendRequest(MsgType type, T &obj) {

  if (mode == "Backtest") {
    //goes to backtester
    //backtester.sendRequest();
  } else if (mode == "LiveTrading") {
    //setup msghub
  } else {
    LOG(FATAL) << "Invalid mode " << mode << "check your config please"; 
  }

  return 0;
}
