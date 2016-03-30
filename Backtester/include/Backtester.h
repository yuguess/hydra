#ifndef BACKTESTER_H
#define BACKTESTER_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "OrderBook.h"

#include <queue>
#include <chrono>
#include <boost/date_time/posix_time/posix_time.hpp>

class Backtester {

public:
  int run();
  int sendRequest(OrderRequest &req);

  //register callback
  int registerCallback(ProtoBufMsgHub::MsgCallback callback) {
    msgCallback = callback;
    return 0;
  }


private:
  static boost::posix_time::ptime toTimestamp(MarketUpdate &);

  struct LessThanByTimestamp {
    bool operator()(MarketUpdate &left, MarketUpdate &right) const {
      return (::Backtester::toTimestamp(left)
          < ::Backtester::toTimestamp(right));
    }
  };

  std::map<std::string, std::shared_ptr<OrderBook>> orderbooks;
  std::priority_queue<MarketUpdate,
    std::vector<MarketUpdate>, LessThanByTimestamp> pq;
  const static int precisionStep = 10;
  ProtoBufMsgHub::MsgCallback msgCallback;
};

#endif
