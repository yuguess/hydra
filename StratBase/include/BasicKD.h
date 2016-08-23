#ifndef BASIC_KD_H
#define BASIC_KD_H

#include "RangeStat.h"
#include "StratBase.h"
#include "json/json.h"

class BasicKD: public StratBase {
public:
  BasicKD();
  virtual ~BasicKD() {};
  int onMsg(MessageBase&);

private:
  bool onCreate();
  bool onExit();

  RangeStatCollector fiveMinStat;
  Json::Value jsonState;
};

#endif
