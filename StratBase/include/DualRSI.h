#ifndef DUAL_RSI_H
#define DUAL_RSI_H

#include "StratBase.h"
#include "json/json.h"
#include "TransacLogger.h"
#include "RSI.h"

class DualRSI : public StratBase {
public:
  DualRSI() : qRSI(4), sRSI(6) {};
  virtual ~DualRSI() {};
  int onMsg(MessageBase&);

private:
  bool onCreate();
  bool onExit();

  TransacLogger transacLogger;
  Json::Value jsonState;
  RSI qRSI, sRSI;
};

#endif
