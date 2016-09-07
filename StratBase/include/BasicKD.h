#ifndef BASIC_KD_H
#define BASIC_KD_H

#include "TransacLogger.h"
#include "RangeStatCollector.h"
#include "StratBase.h"
#include "json/json.h"
#include "KDJ.h"

class BasicKD: public StratBase {
public:
  BasicKD();
  ~BasicKD() {
    onExit();
  };

  int onMsg(MessageBase&);

private:
  bool onCreate();
  bool onExit();

  int onRangeStatUpdate(RangeStat&);
  bool enterMarket(std::string, std::string, int, double, std::string);
  OrderRequest getInitOrderRequest();

  TransacLogger transacLogger;
  KD kd;
  Json::Value jsonState;
  RangeCollector fiveMinData;
  std::string respAddr;
};

#endif
