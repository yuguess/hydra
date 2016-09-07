#ifndef DATA_ADAPTER_FACTORY_H
#define DATA_ADAPTER_FACTORY_H

#include "BasicFuturesDataAdapter.h"
#include "BasicStockDataAdapter.h"
#include "RangeDataAdapter.h"
#include "DayDataAdapter.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"

class DataAdapterFactory {

public:
  static std::shared_ptr<DataAdapter> createAdapter(std::string stream,
      Json::Value &jsonConfigObj) {

    std::shared_ptr<DataAdapter> ptr;
    std::string adapterType = jsonConfigObj["Type"].asString();
    if (adapterType == "BasicFuturesDataAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new BasicFuturesDataAdapter());
    } else if (adapterType == "BasicStockDataAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new BasicStockDataAdapter());
    } else if (adapterType == "RangeDataAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new RangeDataAdapter());
    } else if (adapterType == "DayDataAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new DayDataAdapter());
    }  else {
      LOG(FATAL) << "Unsupport Data Adapter " << adapterType;
      return std::shared_ptr<DataAdapter>(NULL);
    }
    ptr->init(stream, jsonConfigObj);

    return ptr;
  }
};

#endif
