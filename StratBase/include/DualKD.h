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
  bool enterMarket(std::string, std::string, double, std::string);
  bool onCreate();
  bool onRangeStatUpdate(RangeStat&);
  bool onDayUpdate(RangeStat&);

  KD qKD, sKD;
  RangeCollector rangeCollector;

  TransacLogger transacLogger;
  Json::Value jsonState;

  std::string respAddr;
};

#endif
