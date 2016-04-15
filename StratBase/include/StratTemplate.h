#ifndef STRAT_TEMPLATE_H
#define STRAT_TEMPLATE_H

#include "StratBase.h"
#include "RangeStat.h"
#include "PositionManager.h"

class StratTemplate : public StratBase {
public:
  StratTemplate();
  ~StratTemplate() {};
  int onMsg(MessageBase&);

private:
  std::string tradeSecurity;
  int twoMinUpdate(RangeStatData &rng);
  RangeStat twoMin;
  PositionManager positionManager;
};

#endif
