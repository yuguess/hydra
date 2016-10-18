#ifndef DUAL_RSI_H
#define DUAL_RSI_H

#include "StratBase.h"
#include "json/json.h"
#include "TransacLogger.h"
#include "RSI.h"

class DualRSI : public StratBase {
public:
  DualRSI() : slowRSIThre(0.45), quickRSIThre(0.65), qRSI(4), sRSI(6),
    qRSIVal(0.0), sRSIVal(0.0), curPosition(0), sRSIValValidFlag(false) {
  }

  virtual ~DualRSI() {
    onExit();
  }

  int onMsg(MessageBase&);

private:
  double slowRSIThre, quickRSIThre;

  bool onCreate();
  bool onExit();
  bool onRangeStatUpdate(RangeStat&);

  bool isBelowTimeThre(std::string ts);
  bool enterMarket(std::string, std::string, double, std::string);
  bool flatAll(std::string, double, std::string);

  TransacLogger transacLogger;
  Json::Value jsonState;
  RSI qRSI, sRSI;
  double qRSIVal, sRSIVal;
  int curPosition;
  bool sRSIValValidFlag;
};

#endif
