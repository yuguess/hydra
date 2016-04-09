#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include <vector>
#include <boost/heap/fibonacci_heap.hpp>

class OrderBook {
public:
  OrderBook(int);
  int screenshotUpdate(MarketUpdate &mktUpdt);
  int sendOrder(OrderRequest &);
  int registerCallback(ProtoBufMsgHub::MsgCallback callback) {
    msgCallback = callback;
    return 0;
  }

private:
  int sendResp(OrderRequest &);

  struct LessThanByAsk {
    bool operator()(const OrderRequest &left, const OrderRequest &right) const {
      return (left.limit_price() < right.limit_price());
    }
  };

  struct MoreThanByBid {
    bool operator()(const OrderRequest &left, const OrderRequest &right) const {
      return (left.limit_price() > right.limit_price());
    }
  };

  boost::heap::fibonacci_heap<OrderRequest,
    boost::heap::compare<OrderBook::MoreThanByBid>> bidLimitOrders;
  boost::heap::fibonacci_heap<OrderRequest,
    boost::heap::compare<OrderBook::LessThanByAsk>> askLimitOrders;

  int respId;
  ProtoBufMsgHub::MsgCallback msgCallback;
  std::vector<double> bidPrice, askPrice;
  std::vector<int> bidVolume, askVolume;
};

#endif
