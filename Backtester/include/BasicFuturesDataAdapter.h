#ifndef BASIC_FUTURES_DATA_ADAPTER_H
#define BASIC_FUTURES_DATA_ADAPTER_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "DataAdapter.h"

class BasicFuturesDataAdapter : public DataAdapter {

public:
  int getNextData(MarketUpdate &mktUpdt) {
    return 0;
  }

  int close() {
    return 0;
  }
};

#endif
