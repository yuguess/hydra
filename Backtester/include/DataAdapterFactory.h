#ifndef DATA_ADAPTER_FACTORY_H
#define DATA_ADAPTER_FACTORY_H

#include "BasicFuturesDataAdapter.h"
#include "BasicStockDataAdapter.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"

class AdapterFactory {

public:
  static std::shared_ptr<DataAdapter> 
    createAdapter(std::string adapterType, 
      std::string code, std::string &argList) {

    std::shared_ptr<DataAdapter> ptr;
    if (adapterType == "BasicFuturesDataAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new BasicFuturesDataAdapter()); 
    } else if (adapterType == "BasicStockDataAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new BasicStockDataAdapter());
    } else {
      LOG(FATAL) << "Unsupport Data Adapter";
      return std::shared_ptr<DataAdapter>(NULL);
    }

    ptr->initWithArgList(code, argList);
  }
};

#endif
