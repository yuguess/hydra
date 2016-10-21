#ifndef BACKTESTER_H
#define BACKTESTER_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "OrderBook.h"
#include "DataAdapter.h"

#include <queue>
#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>

class Backtester {

public:
  int run(Json::Value&);
  int onRequest(OrderRequest &req);

  //register callback
  int registerCallback(ProtoBufMsgHub::MsgCallback callback) {
    msgCallback = callback;
    return 0;
  }

private:
  static boost::posix_time::ptime getTodayEnd() {
    boost::gregorian::days oneDay(1);

    std::string dateStr =
      boost::gregorian::to_iso_extended_string(
          boost::gregorian::day_clock::local_day());

    boost::posix_time::ptime curTm =
      boost::posix_time::time_from_string(dateStr + " 00:00:00.000");

    return curTm + oneDay;
  }

  struct LessThanByTimestamp {
    bool operator()(TimeSeriesData&left, TimeSeriesData &right) const {
      return (left.ts > right.ts);
    }
  };

  std::map<std::string, std::shared_ptr<DataAdapter>> nameToAdapter;
  std::map<std::string, std::shared_ptr<OrderBook>> orderbooks;
  std::priority_queue<TimeSeriesData,
    std::vector<TimeSeriesData>, LessThanByTimestamp> pq;
  const static int precisionStep = 10;
  ProtoBufMsgHub::MsgCallback msgCallback;
};

#endif
