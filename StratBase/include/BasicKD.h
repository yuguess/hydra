#ifndef BASIC_KD_H
#define BASIC_KD_H

#include "StratBase.h"

class BasicKD: public StratBase {
public:
  BasicKD() {};
  virtual ~BasicKD() {};
  int onMsg(MessageBase&);
};

#endif
