#ifndef BASIC_RSI_H
#define BASIC_RSI_H

#include "StratBase.h"

class BasicRSI : public StratBase {
public:
  BasicRSI() {};
  virtual ~BasicRSI() {};
  int onMsg(MessageBase&);
};

#endif
