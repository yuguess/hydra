#include "FirstLevelOrder.h"
#include "CedarHelper.h"

FirstLevelOrder::FirstLevelOrder(OrderRequest &req,
    SmartOrderService *srcv):OrderReactor(req, srcv),
    orderState(OrderState::Ready) {

  respAddr = CedarHelper::getResponseAddr();
  leftQty = req.trade_quantity();
}

int FirstLevelOrder::onMsg(MessageBase &msg) {

  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mkt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    double currentLevelPrice = 0.0;
    int currentLevelVol = 0;

    if (orderRequest.buy_sell() == LONG_BUY) {
      currentLevelPrice = mkt.bid_price(0);
      currentLevelVol = mkt.bid_volume(0);
    } else {
      currentLevelPrice = mkt.ask_price(0);
      currentLevelVol =mkt.ask_volume(0);
    }

    if (orderState == OrderState::Ready) {
      outOrderId = CedarHelper::getOrderId();
      placePrice = currentLevelPrice;

      //orderRequest is from original orderRequest
      OrderRequest req = orderRequest;
      req.set_response_address(respAddr);
      req.set_type(TYPE_LIMIT_ORDER_REQUEST);
      req.set_limit_price(currentLevelPrice);
      req.set_id(outOrderId);
      req.set_buy_sell(orderRequest.buy_sell());

      if (orderRequest.buy_sell() == LONG_BUY) {
        if (CedarHelper::isStock(req.code())) {
          //round up for buying
          leftQty /= stockMinimumQty;
          leftQty = leftQty * stockMinimumQty + stockMinimumQty;
        }
      } else if (orderRequest.buy_sell() == SHORT_SELL) {
        //round  down for selling
        if (CedarHelper::isStock(req.code())) {
          leftQty /= stockMinimumQty;
          leftQty *= stockMinimumQty;
          if (leftQty == 0) {
            LOG(INFO) << "sell again with qty < 100, just recycle";
            setRecycle();
            return 0;
          }
        }
      }
      req.set_trade_quantity(leftQty);

      //usually caused by reaching highlimit or lowlimit
      if (req.limit_price() == 0.0) {
        LOG(INFO) << "probably touching highlimit or lowlimit";
        setRecycle();
        return 0;
      }

      service->sendRequest(getOrderReactorID(), req);
      orderState = Sending;

      LOG(INFO) << "firstLevel send order complete";
      return 0;
    } else if (orderState == OrderState::Sent) {
      //seocnd condition, cancel if I am the only one in the front level
      if (placePrice != currentLevelPrice ||(placePrice == currentLevelPrice &&
         orderRequest.trade_quantity() == currentLevelVol)) {

        cancelOrder();
        orderState = Canceling;
      }
    }

  } else if (msg.type() == TYPE_RESPONSE_MSG) {
    ResponseMessage rsp=ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);

    if (rsp.type() == TYPE_NEW_ORDER_CONFIRM) {
      orderState = Sent;
    } else if (rsp.type() == TYPE_CANCEL_ORDER_CONFIRM) {
      if (orderState == TerminateCancel) {
        setRecycle();
        return 0;
      }

      orderState = Ready;

    } else if (rsp.type() == TYPE_TRADE) {
      leftQty -= rsp.trade_quantity();
      if (CedarHelper::isStock(orderRequest.code())
          && leftQty < stockMinimumQty) {

        if (leftQty == 0) {
          setRecycle();
          return 0;
        }

        cancelOrder();
        orderState = TerminateCancel;
      }
    } else if (rsp.type() == TYPE_ERROR) {
      //Enter terminate process
    }
  }

  return 0;
}

int FirstLevelOrder::cancelOrder() {
  OrderRequest req = orderRequest;
  req.set_id(CedarHelper::getOrderId());
  req.set_response_address(respAddr);
  req.set_type(TYPE_CANCEL_ORDER_REQUEST);
  req.set_cancel_order_id(outOrderId);
  service->sendRequest(getOrderReactorID(), req);
  return 0;
}
