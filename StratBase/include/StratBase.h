#ifndef STRAT_BASE_H
#define STRAT_BASE_H

#include "CedarJsonConfig.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "Backtester.h"
#include "PersistentState.h"

class StratBase {
public:
  int run();

  virtual bool onCreate() = 0;
  virtual bool onExit() = 0;
  virtual int onMsg(MessageBase&) = 0;

  template<typename T>
  int sendRequest(CedarMsgType type, T &obj) {
    switch (mode) {
      case BACKTEST:
        LOG(INFO) << "order to backtester";
        //goes to backtester
        //backtester.sendRequest();
      case LIVETEST:
        LOG(INFO) << "order to virtual matching engine";
        break;
        //send through orderAgent
      case LIVE_TRADING:
        LOG(INFO) << "order to backtester";
       //send through orderAgent
      default:
        LOG(FATAL) << "Invalid Strategy Mode " << mode;
    }

    return 0;
  }

protected:
  inline StrategyMode getStrategyMode() const {
    return mode;
  }

  //orderAgent

private:
  int onMsgWrapper(MessageBase);

  StrategyMode mode;
  ProtoBufMsgHub msgHub;
  Backtester backtester;
};

#endif
