#ifndef DATA_ADAPTER_FACTORY_H
#define DATA_ADAPTER_FACTORY_H

#include "BasicFuturesDataAdapter.h"
#include "BasicStockDataAdapter.h"
#include "RangeDataAdapter.h"
#include "DayDataAdapter.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "GTAAdapter.h"

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
    } else if (adapterType == "GTATaqAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new GTAAdapter<TaqData>());
    } else if (adapterType == "GTAIndexAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new GTAAdapter<IndexData>());
    } else if (adapterType == "GTATrdMinAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new GTAAdapter<TrdMinData>());
    } else if (adapterType == "GTAOrderqueueAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new GTAAdapter<OrderqueueData>());
    } else if (adapterType == "GTATransactionAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new GTAAdapter<TransactionData>());
    } else if (adapterType == "GTAVirtualauctionAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new GTAAdapter<VirtualauctionData>());
    } else if (adapterType == "GTABulletinAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new GTAAdapter<BulletinData>());
    } else if (adapterType == "GTAOrderAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new GTAAdapter<OrderData>());
    } else if (adapterType == "GTAStockinfoAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new GTAAdapter<StockinfoData>());
    } else if (adapterType == "GTAStockstatusAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new GTAAdapter<StockstatusData>());
    } else if (adapterType == "GTATradeAdapter") {
      ptr = std::shared_ptr<DataAdapter>(new GTAAdapter<TradeData>());
    } else {
      LOG(FATAL) << "Unsupport Data Adapter " << adapterType;
      return std::shared_ptr<DataAdapter>(NULL);
    }
    ptr->init(stream, jsonConfigObj);

    return ptr;
  }
};

#endif
