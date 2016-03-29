#ifndef DATA_ADAPTER_FACTORY_H
#define DATA_ADAPTER_FACTORY_H

#include "BasicFuturesDataAdapter.h"
#include "BasicStockDataAdapter.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"

class AdapterFactory {

public:
  static std::shared_ptr<DataAdapter> createAdapter(std::string adapterType) {
    if (adapterType == "BasicFuturesDataAdapter") {
      return std::shared_ptr<DataAdapter>(new BasicFuturesDataAdapter()); 
    } else if (adapterType == "BasicStockDataAdapter") {
      return std::shared_ptr<DataAdapter>(new BasicStockDataAdapter());
    } else {
      LOG(FATAL) << "Unsupport Data Adapter"; 
      return std::shared_ptr<DataAdapter>(NULL);
    }
  }
};

#endif
