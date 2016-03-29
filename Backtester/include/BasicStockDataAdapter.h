#ifndef BASIC_STOCK_DATA_ADAPTER_H
#define BASIC_STOCK_DATA_ADAPTER_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "DataAdapter.h"

class BasicStockDataAdapter : public DataAdapter {
public:
  int getNextData(MarketUpdate &mktUpdt) {
    return 0;
  }

  int close() {

    return 0;
  }
};

#endif
