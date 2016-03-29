#ifndef DATA_ADAPTER_H
#define DATA_ADAPTER_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"

class DataAdapter {

public:
  virtual int getNextData(MarketUpdate &) = 0;
  virtual int close() = 0;
};

#endif
