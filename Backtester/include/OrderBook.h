#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include <vector>

class OrderBook {
public:
  OrderBook::OrderBook(int);
  int screenshotUpdate(MarketUpdate &mktUpdt);
  int sendOrder(OrderRequest &);
  int registerCallback(ProtoBufMsgHub::MsgCallback callback) {
    msgCallback = callback;
    return 0;
  }

private:
  struct LessThanByAskPrice {
    bool operator()(MarketUpdate &left, MarketUpdate &right) const {
      return (left.limitPrice() < right.limitPrice());
    }
  };

  struct MoreThanByBid {
    bool operator()(OrderRequest &left, OrderRequest &right) const {
      return (left.limitPrice() > right.limitPrice());
    }
  };

  std::priority_queue<OrderRequest,
    std::vector<OrderRequest>, LessThanByAskPrice> bidLimitOrders;
  std::priority_queue<OrderRequest,
    std::vector<OrderRequest>, MoreThanByBidPrice> askLimitOrders;

  ProtoBufMsgHub::MsgCallback msgCallback;
  std::vector<double> bidPrice, askPrice;
  std::vector<int> bidVolume, askVolume;
};

#endif
