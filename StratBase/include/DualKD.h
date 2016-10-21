#ifndef DUAL_KD_H
#define DUAL_KD_H

#include "StratBase.h"
#include "json/json.h"
#include "TransacLogger.h"
#include "KDJ.h"
#include "RangeStatCollector.h"

class DualKD : public StratBase {

public:
  DualKD();
  bool onExit();
  int onMsg(MessageBase&);

private:
  const static int UNINITIALIZE = -100;
  const static int SLOW_BUY = 1;
  const static int SLOW_SELL = -1;

  bool enterMarket(TradeDirection, std::string, double, std::string);
  bool onCreate();
  bool onRangeStatUpdate(RangeStat&);
  bool onDayUpdate(RangeStat&);

  KD qKD, sKD;
  RangeCollector rangeCollector;

  TransacLogger transacLogger;
  Json::Value jsonState;

  std::string respAddr;

  int slowSignal;
  double preSlowK, preQuickK;
};

#endif
