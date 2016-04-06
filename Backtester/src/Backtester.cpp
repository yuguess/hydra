#include "Backtester.h"
#include "CedarJsonConfig.h"
#include "DataAdapter.h"
#include "DataAdapterFactory.h"

#include <iostream>
#include <vector>

int Backtester::run() {

  std::vector<std::string> codes;
  std::vector<std::string> adapterTypes;
  CedarJsonConfig::getInstance().getStringArrayWithTag(codes,
    "Backtester.Ticker", "code");
  CedarJsonConfig::getInstance().getStringArrayWithTag(adapterTypes,
    "Backtester.Ticker", "adapter");

  std::vector<std::shared_ptr<DataAdapter>> adapters;
  for (unsigned int i = 0; i < codes.size(); i++) {
    adapters.push_back(AdapterFactory::createAdapter(adapterTypes[i]));
    orderbooks[codes[i]] = std::shared_ptr<OrderBook>(new OrderBook);
    orderbooks[codes[i]]->registerCallback(msgCallback);
  }

  for (unsigned int i = 0; i < adapters.size(); i++ ) {
    MarketUpdate mktUpdt;
    if (adapters[i]->getNextData(mktUpdt) != 0) {
      LOG(ERROR) << "there is a emtpy data soruce";
    } else {
      pq.push(mktUpdt);
    }
  }

  MarketUpdate topMkt = pq.top();
  boost::posix_time::ptime curTimestamp = toTimestamp(topMkt);
  boost::posix_time::ptime topTimestamp = curTimestamp;

  do {
    while (topTimestamp <= curTimestamp) {
      orderbooks[topMkt.code()]->screenshotUpdate(topMkt);

      try {
        msgCallback(ProtoBufHelper::toMessageBase<MarketUpdate>
            (TYPE_MARKETUPDATE, topMkt));
      } catch (const std::bad_function_call &e) {
        LOG(ERROR) << "recv msg but msghub doesn't have register callback";
        LOG(ERROR) << e.what();
      } catch (...) {
        LOG(ERROR) << "Msghub callback function error !";
      }

      pq.pop();
      topMkt = pq.top();
      topTimestamp = toTimestamp(topMkt);

      //if (leap a day) {
      // day event
      //}

    }
    curTimestamp += boost::posix_time::millisec(precisionStep);
  } while (true);

  return 0;
}

int Backtester::sendRequest(OrderRequest &req) {
  if (orderbooks.find(req.code()) == orderbooks.end()) {
    LOG(FATAL) << " send an invalid order, code " << req.code() 
              << " does not have order book ";
    return -1;
  }

  orderbooks[req.code()]->sendOrder(req); 
  return 0;
}
