#ifndef DUAL_KD_H
#define DUAL_KD_H

#include "StratBase.h"
#include "json/json.h"
#include "TransacLogger.h"
#include "KDJ.h"

class DualKD : public StratBase {

public:
  DualKD() : qKD(13, 3, 3), sKD(13, 3, 3) {}
  ~DualKD() {
    onExit();
  }
  bool onExit();
  int onMsg(MessageBase&);

private:
  bool enterMarket(std::string, std::string, double, std::string);
  bool onCreate();

  TransacLogger transacLogger;
  Json::Value jsonState;
  KD qKD, sKD;
};

#endif
