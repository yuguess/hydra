#ifndef STRAT_BASE_H
#define STRAT_BASE_H

#include "CedarJsonConfig.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "Backtester.h"
#include "PersistentState.h"
#include "OrderAgent.h"

class StratBase {
public:
  StratBase();
  int run();

protected:
  virtual bool onCreate() = 0;
  virtual bool onExit() = 0;
  virtual int onMsg(MessageBase&) = 0;

  inline StrategyMode getStrategyMode() const {
    return mode;
  }

  OrderAgent orderAgent;

private:
  int subscribeTicker(std::string, std::string);
  int onMsgWrapper(MessageBase);
  bool setupLiveTestConfig(Json::Value&);
  bool getBacktestRangeDateStr(int, std::string&, std::string&);

  std::string stkDataAddr, stkBoardcastAddr;
  std::string futuresDataAddr, futuresBoardcastAddr;
  std::vector<std::string> tickers, exchanges;

  StrategyMode mode;
  ProtoBufMsgHub msgHub;
  Backtester backtester;
};

#endif
