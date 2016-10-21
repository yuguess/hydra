#include "Backtester.h"
#include "CedarJsonConfig.h"
#include "DataAdapter.h"
#include "DataAdapterFactory.h"
#include "JsonHelper.h"

#include <iostream>
#include <vector>

int Backtester::run(Json::Value &jsonConf) {
  std::vector<std::string> streams;
  JsonHelper::getStringArrayWithTag(jsonConf, "Streams", "", streams);

  for (unsigned i = 0; i < streams.size(); i++) {
    Json::Value adapterJsonConfig;
    JsonHelper::getJsonValueByPath(jsonConf, streams[i], adapterJsonConfig);
    nameToAdapter[streams[i]] =
      DataAdapterFactory::createAdapter(streams[i], adapterJsonConfig);
  }

  //std::map<std::string, std::shared_ptr<DataAdapter>> streams;
  //for (unsigned int i = 0; i < streams.size(); i++) {
    //adapters[codes[i]] =
      //AdapterFactory::createAdapter(adapterTypes[i], codes[i], arglists[i]);

  //Json::Value val;
  //CedarJsonConfig::getInstance().getJsonValueByPath("Backtest.IF", val);
  //LOG(INFO) << val["Type"];

  //  std::string level;
  //  CedarJsonConfig::getInstance().getStringByPath(
  //       adapterTypes[i] + ".Level", level);

  //  orderbooks[codes[i]] = std::shared_ptr<OrderBook>(
  //      new OrderBook(std::stoi(level)));
  //  orderbooks[codes[i]]->registerCallback(msgCallback);
  //}

  for (auto it = nameToAdapter.begin(); it != nameToAdapter.end(); it++) {
    TimeSeriesData tsData;
    if (it->second->getNextData(tsData)) {
      LOG(INFO) << it->first;
      pq.push(tsData);
    } else {
      LOG(ERROR) << "data stream " << it->first << " is empty ";
    }
  }

  while (not pq.empty()) {
    TimeSeriesData top = pq.top();
    try {
      msgCallback(top.msg);
    } catch (const std::bad_function_call &e) {
      LOG(ERROR) << "recv msg but msghub doesn't have"
                 <<"register callback";
      LOG(ERROR) << e.what();
    } catch (...) {
      LOG(ERROR) << "Msghub callback function error !";
    }
    pq.pop();
    TimeSeriesData tsData;
    if (nameToAdapter[top.streamName]->getNextData(tsData)) {
      pq.push(tsData);
    } else {
      LOG(INFO) << "finish stream " << top.streamName;
    }
  }

  //TimeSeriesData topMkt = pq.top();
  //boost::posix_time::ptime curTimestamp = toTimestamp(topMkt);
  //boost::posix_time::ptime topTimestamp = curTimestamp;
  //static boost::posix_time::ptime todayEnd = getTodayEnd();

  //do {
  //  while (topTimestamp <= curTimestamp) {
  //    orderbooks[topMkt.code()]->screenshotUpdate(topMkt);

  //    try {
  //      msgCallback(ProtoBufHelper::toMessageBase<MarketUpdate>
  //          (TYPE_MARKETUPDATE, topMkt));
  //    } catch (const std::bad_function_call &e) {
  //      LOG(ERROR) << "recv msg but msghub doesn't have"
  //                 <<"register callback";
  //      LOG(ERROR) << e.what();
  //    } catch (...) {
  //      LOG(ERROR) << "Msghub callback function error !";
  //    }

  //    pq.pop();

  //    //get its data adapter getNextData and push into pq
  //    if (adapters[topMkt.code()]->getNextData(topMkt) == -1)
  //      LOG(INFO) << "code " << topMkt.code() << " Backtest data complete";
  //    else
  //      pq.push(topMkt);

  //    //check if zero
  //    if (pq.empty()) {
  //      LOG(INFO) << "Backtest data complete ";
  //      break;
  //    }

  //    topMkt = pq.top();
  //    topTimestamp = toTimestamp(topMkt);

  //    //if (leap a day) {
  //    // day event
  //    //}
  //  }
  //  curTimestamp += boost::posix_time::millisec(precisionStep);
  //} while (curTimestamp <= todayEnd);

  return 0;
}

int Backtester::onRequest(OrderRequest &req) {
  if (orderbooks.find(req.code()) == orderbooks.end()) {
    LOG(FATAL) << " send an invalid order, code " << req.code()
              << " is not in order book ";
    return -1;
  }

  orderbooks[req.code()]->sendOrder(req);
  return 0;
}
