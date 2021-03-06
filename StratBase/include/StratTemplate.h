#ifndef STRAT_TEMPLATE_H
#define STRAT_TEMPLATE_H

#include "StratBase.h"
#include "RangeStat.h"
#include "PositionManager.h"
#include "OrderDelegate.h"

class StratTemplate : public StratBase {
public:
  StratTemplate();
  ~StratTemplate() {};
  int onMsg(MessageBase&);

private:
  std::string tradeSecurity, exchange;
  int twoMinUpdate(RangeStatData &rng);
  RangeStat twoMin;
  PositionManager positionManager;
  OrderDelegate orderDelegate;
};

#endif
