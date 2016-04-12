#ifndef STRAT_TEMPLATE_H
#define STRAT_TEMPLATE_H

#include "StratBase.h"
#include "RangeStat.h"

class StratTemplate : public StratBase {
public:
  int onMsg(MessageBase&);
  ~StratTemplate() {};

private:
  int fiveMinUpdate(RangeStatData &rng);

};

#endif
