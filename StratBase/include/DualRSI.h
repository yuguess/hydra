#ifndef DUAL_RSI_H
#define DUAL_RSI_H

#include "StratBase.h"

class DualRSI : public StratBase {
public:
  DualRSI() {};
  virtual ~DualRSI() {};
  int onMsg(MessageBase&);
};

#endif
