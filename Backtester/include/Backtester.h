#ifndef BACKTESTER_H
#define BACKTESTER_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"

#include <queue>

class Backtester {

public:
  int run();
  //int sendRequest();
  //register callback

private:
  struct LessThanByTimestamp {
    bool operator()(MarketUpdate &left, MarketUpdate &right) const {
      return (left.trading_day() + left.exchange_timestamp()) < 
        (right.trading_day() + right.exchange_timestamp());
    }
  };

  std::priority_queue<MarketUpdate, 
    std::vector<MarketUpdate>, LessThanByTimestamp> pq;
};

#endif
