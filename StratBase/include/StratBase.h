#ifndef STRAT_BASE_H
#define STRAT_BASE_H

#include "CedarJsonConfig.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "Backtester.h"

class StratBase {
public:
  virtual int onMsg(MessageBase&) = 0;

  template<typename T>
  int sendRequest(MsgType type, T &obj) {
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

  int run();

private:
  int onMsgWrapper(MessageBase);
  std::string mode;
  ProtoBufMsgHub msgHub;
  Backtester backtester;
};

#endif
