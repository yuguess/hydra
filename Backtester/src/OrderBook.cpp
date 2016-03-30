#include "OrderBook.h"


OrderBook::OrderBook(int ladderLevel) {
  bidPrice.resize(ladderLevel);
  askPrice.resize(ladderLevel);
  bidVolume.resize(ladderLevel);
  askVolume.resize(ladderLevel);
}

int OrderBook::sendOrder(OrderRequest &orderReq) {
  if (orderReq.type() == TYPE_LIMIT_ORDER_REQUEST) {
    if (orderReq. == ) {

    } else if () {

    } else {

    }

  } else if (orderReq.type() == TYPE_CANCEL_ORDER_REQUEST) {

  } else if (orderReq.type() == TYPE_MARKET_ORDER_REQUEST) {

  } else {
    LOG(ERROR) << "Unsupport order type " << orderReq.type();  
  }

  return 0;
}

int OrderBook::screenshotUpdate(MarketUpdate &mktUpdt) {
  //bidOrders;
  //
  return 0;
}
