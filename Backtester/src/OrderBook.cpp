#include "OrderBook.h"


OrderBook::OrderBook(int ladderLevel) {
  bidPrice.resize(ladderLevel);
  askPrice.resize(ladderLevel);
  bidVolume.resize(ladderLevel);
  askVolume.resize(ladderLevel);
}

int OrderBook::sendOrder(OrderRequest &orderReq) {
  if (orderReq.type() == TYPE_LIMIT_ORDER_REQUEST) {
    if (orderReq.buy_sell() == LONG_BUY) {
      bidLimitOrders.push(orderReq);
    } else if (orderReq.buy_sell() == SHORT_SELL) {
      askLimitOrders.push(orderReq);
    } else {
      LOG(ERROR) << "Invalid buy or sell flag " << orderReq.buy_sell();
    }

  } else if (orderReq.type() == TYPE_CANCEL_ORDER_REQUEST) {

  } else if (orderReq.type() == TYPE_MARKET_ORDER_REQUEST) {

  } else {
    LOG(ERROR) << "Unsupport order type " << orderReq.type();
  }

  return 0;
}

int OrderBook::screenshotUpdate(MarketUpdate &mktUpdt) {
  OrderRequest tradeOrder;

  while (bidLimitOrders.top().limit_price() >= mktUpdt.ask_price(0)) {
    tradeOrder = bidLimitOrders.top();
    bidLimitOrders.pop();

    ResponseMessage tradeRsp;
    tradeRsp.set_ref_id(tradeOrder.id());
    tradeRsp.set_code(tradeOrder.code());
    tradeRsp.set_price(tradeOrder.limit_price());
    tradeRsp.set_trade_quantity(tradeOrder.trade_quantity());

    //send traded response
    //msgCallback();
  }

  while (askLimitOrders.top().limit_price() <= mktUpdt.bid_price(0)) {
    tradeOrder = askLimitOrders.top(); 
    askLimitOrders.pop();
    //send traded response
    //msgCallback();
  }

  return 0;
}
