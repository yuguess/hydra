#include "Backtester.h"
#include "CedarJsonConfig.h"
#include "DataAdapter.h"
#include "DataAdapterFactory.h"

#include <iostream>
#include <vector>

int Backtester::run() {

  std::vector<std::string> codes, adapterTypes, arglists;
  CedarJsonConfig::getInstance().getStringArrayWithTag(codes,
    "Backtester.Ticker", "Code");
  CedarJsonConfig::getInstance().getStringArrayWithTag(adapterTypes,
    "Backtester.Ticker", "Adapter");
  CedarJsonConfig::getInstance().getStringArrayWithTag(arglists,
    "Backtester.Ticker", "Arglist");

  std::map<std::string, std::shared_ptr<DataAdapter>> adapters;
  for (unsigned int i = 0; i < codes.size(); i++) {
    adapters[codes[i]] =
      AdapterFactory::createAdapter(adapterTypes[i], codes[i], arglists[i]);

    std::string level;
    CedarJsonConfig::getInstance().getStringByPath(
         adapterTypes[i] + ".Level", level);

    orderbooks[codes[i]] = std::shared_ptr<OrderBook>(
        new OrderBook(std::stoi(level)));
    orderbooks[codes[i]]->registerCallback(msgCallback);
  }

  for (auto it = adapters.begin(); it != adapters.end(); it++) {
    MarketUpdate mktUpdt;
    if (adapters[it->first]->getNextData(mktUpdt) != 0) {
      LOG(ERROR) << "there is a emtpy data soruce";
    } else {
      pq.push(mktUpdt);
    }
  }

  MarketUpdate topMkt = pq.top();
  boost::posix_time::ptime curTimestamp = toTimestamp(topMkt);
  boost::posix_time::ptime topTimestamp = curTimestamp;
  static boost::posix_time::ptime todayEnd = getTodayEnd();

  do {
    while (topTimestamp <= curTimestamp) {
      orderbooks[topMkt.code()]->screenshotUpdate(topMkt);

      try {
        msgCallback(ProtoBufHelper::toMessageBase<MarketUpdate>
            (TYPE_MARKETUPDATE, topMkt));
      } catch (const std::bad_function_call &e) {
        LOG(ERROR) << "recv msg but msghub doesn't have"
                   <<"register callback";
        LOG(ERROR) << e.what();
      } catch (...) {
        LOG(ERROR) << "Msghub callback function error !";
      }

      pq.pop();

      //get its data adapter getNextData and push into pq
      if (adapters[topMkt.code()]->getNextData(topMkt) == -1)
        LOG(INFO) << "code " << topMkt.code() << " Backtest data complete";
      else
        pq.push(topMkt);

      //check if zero
      if (pq.empty()) {
        LOG(INFO) << "Backtest data complete ";
        break;
      }

      topMkt = pq.top();
      topTimestamp = toTimestamp(topMkt);

      //if (leap a day) {
      // day event
      //}
    }
    curTimestamp += boost::posix_time::millisec(precisionStep);
  } while (curTimestamp <= todayEnd);

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
