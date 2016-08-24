#ifndef BASIC_KD_H
#define BASIC_KD_H

#include "RangeStat.h"
#include "StratBase.h"
#include "json/json.h"
#include "KDJ.h"

class BasicKD: public StratBase {
public:
  BasicKD();
  virtual ~BasicKD() {};
  int onMsg(MessageBase&);

private:
  bool onCreate();
  bool onExit();

  KD kd;
  Json::Value jsonState;
};

#endif
