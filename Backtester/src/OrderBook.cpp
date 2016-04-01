#include "OrderBook.h"


OrderBook::OrderBook(int ladderLevel) : respId(1) {
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
    auto it = std::find_if(bidLimitOrders.begin(), bidLimitOrders.end(), 
        [orderReq](OrderRequest &req) {return req.id() == orderReq.id();});
    if (it != 
    it = std::find_if(bidLimitOrders.begin(), bidLimitOrders.end(), 
        [orderReq](OrderRequest &req) {return req.id() == orderReq.id();});

    LOG(ERROR) << "";
  } else if (orderReq.type() == TYPE_MARKET_ORDER_REQUEST) {

  } else {
    LOG(ERROR) << "Unsupport order type " << orderReq.type();
  }

  return 0;
}

int OrderBook::sendResp(OrderRequest &tradeOrder) {
  ResponseMessage tradeRsp;
  tradeRsp.set_id(std::to_string(respId++));
  tradeRsp.set_ref_id(tradeOrder.id());
  tradeRsp.set_code(tradeOrder.code());
  tradeRsp.set_price(tradeOrder.limit_price());
  tradeRsp.set_trade_quantity(tradeOrder.trade_quantity());

  try {
    msgCallback(ProtoBufHelper::toMessageBase<ResponseMessage>
      (TYPE_RESPONSE_MSG, tradeRsp));
  } catch (const std::bad_function_call &e) {
    LOG(ERROR) << "recv msg but msghub doesn't have register callback";
    LOG(ERROR) << e.what();
  } catch (...) {
    LOG(ERROR) << "Msghub callback function error !";
  }
}

int OrderBook::screenshotUpdate(MarketUpdate &mktUpdt) {
  OrderRequest tradeOrder;

  while (bidLimitOrders.top().limit_price() >= mktUpdt.ask_price(0)) {
    tradeOrder = bidLimitOrders.top();
    bidLimitOrders.pop();

    sendResp(tradeOrder);
  }

  while (askLimitOrders.top().limit_price() <= mktUpdt.bid_price(0)) {
    tradeOrder = askLimitOrders.top();
    askLimitOrders.pop();

    sendResp(tradeOrder);
  }

  return 0;
}
