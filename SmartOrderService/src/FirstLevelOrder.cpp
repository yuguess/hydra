#include "FirstLevelOrder.h"
#include "CedarHelper.h"
#include "MarketUpdateHelper.h"
#include "MarketSpecHelper.h"

FirstLevelOrder::FirstLevelOrder(OrderRequest &req,
    SmartOrderService *srcv):OrderReactor(req, srcv),
    orderState(Init), activeMode(false) {

  ptimeThre = getTimeThre();
  oneTick = MarketSpecHelper::getOneTick(req.code());

  respAddr = CedarHelper::getResponseAddr();
  leftQty = req.trade_quantity();
  LOG(INFO) << "leftQty " << leftQty;

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

    switch (orderState) {

      case Init: {
        logStatusInfo(mkt);
        orderState = Ready;
        break;
      }

      case Ready: {
        placePrice = currentLevelPrice;

        if (isInActiveMode(mkt, orderRequest.buy_sell())) {
          activeMode = true;
          if (orderRequest.buy_sell() == LONG_BUY)
            placePrice += oneTick;
          else {
            placePrice -= oneTick;
          }

          LOG(INFO) << "first level switch to active mode";
        }

        sendLimitOrder(placePrice);
        LOG(INFO) << "firstLevel send order complete";
        break;
      }

      case Sent: {

        if (CedarTimeHelper::getCurPTime() > ptimeThre) {
          cancelOrder();
          orderState = ChaseCanceling;
          break;
        }

        if (activeMode) {
          TradeDirection dir = orderRequest.buy_sell();
          double topPrice = (dir == LONG_BUY) ?
            mkt.bid_price(0) : mkt.ask_price(0);

          if (isBreakActiveMode(currentLevelPrice, topPrice, dir) ||
            ((currentLevelPrice == placePrice) &&
             (orderRequest.trade_quantity() != currentLevelVol))) {

            activeMode = false;
          }
        }

        //seocnd condition, cancel if I am the only one in the front level
        if (placePrice != currentLevelPrice ||
          (placePrice == currentLevelPrice &&
           orderRequest.trade_quantity() == currentLevelVol)) {

          if (!activeMode) {
            cancelOrder();
          }

          orderState = Canceling;
        }

        break;
      }

      case ChaseReady: {
        double hitPrice = (orderRequest.buy_sell() == LONG_BUY) ?
            mkt.ask_price(0) : mkt.bid_price(0);

        sendLimitOrder(hitPrice);
        setRecycle();

        break;
      }

      default:break;
    }


  } else if (msg.type() == TYPE_RESPONSE_MSG) {
    ResponseMessage rsp=ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);

    if (rsp.type() == TYPE_NEW_ORDER_CONFIRM) {
      orderState = Sent;
    } else if (rsp.type() == TYPE_CANCEL_ORDER_CONFIRM) {
      if (orderState == TerminateCancel) {
        setRecycle();
        return 0;
      } else if (orderState == ChaseCanceling) {
        orderState = ChaseReady;
      } else {
        orderState = Ready;
      }

    } else if (rsp.type() == TYPE_TRADE) {
      leftQty -= rsp.trade_quantity();
      if (CedarHelper::isStock(orderRequest.code())
          && leftQty < stockMinimumQty) {

        if (leftQty == 0) {
          setRecycle();
          return 0;
        }

        //cancelOrder();
        //orderState = TerminateCancel;
      }

    } else if (rsp.type() == TYPE_ERROR) {
      //Enter terminate process
    }
  }

  return 0;
}

int FirstLevelOrder::cancelOrder() {
  if (orderState == Canceling) {
    LOG(INFO) << "In canceling process, no need to cancel again";
    return -1;
  }

  OrderRequest req = orderRequest;
  req.set_id(CedarHelper::getOrderId());
  req.set_response_address(respAddr);
  req.set_type(TYPE_CANCEL_ORDER_REQUEST);
  req.set_cancel_order_id(outOrderId);
  service->sendRequest(getOrderReactorID(), req);
  return 0;
}

bool FirstLevelOrder::isInActiveMode(MarketUpdate &mkt, TradeDirection dir) {
  int bidVol = MarketUpdateHelper::totalBidVol(mkt);
  int askVol = MarketUpdateHelper::totalAskVol(mkt);
  int topVol = (dir == LONG_BUY) ? mkt.bid_volume(0) : mkt.ask_volume(0);

  if (dir == LONG_BUY) {
    if (bidVol > volMulti * askVol && topVol * topVolMulti> bidVol) {
      LOG(INFO) << "first level switch to active mode for bid";
      return true;
    }
  } else {
    if (askVol > volMulti * bidVol && topVol * topVolMulti> askVol) {
      LOG(INFO) << "first level switch to active mode for ask";
      return true;
    }
  }

  return false;
}

bool FirstLevelOrder::isBreakActiveMode(
  double topPrice, double placePrice, TradeDirection dir) {

  if (dir == LONG_BUY && topPrice > placePrice)
    return true;
  else if (dir == SHORT_SELL && topPrice < placePrice)
    return true;

  return false;
}

bool FirstLevelOrder::sendLimitOrder(double limitPrice) {
  outOrderId = CedarHelper::getOrderId();

  //orderRequest is from original orderRequest
  OrderRequest req = orderRequest;
  req.set_response_address(respAddr);
  req.set_type(TYPE_LIMIT_ORDER_REQUEST);
  req.set_limit_price(limitPrice);
  req.set_id(outOrderId);
  req.set_buy_sell(orderRequest.buy_sell());

  if (orderRequest.buy_sell() == LONG_BUY &&
      CedarHelper::isStock(req.code())) {
      leftQty = CedarHelper::stockQtyRoundUp(leftQty);
  } else if (orderRequest.buy_sell() == SHORT_SELL &&
      CedarHelper::isStock(req.code())) {

    //round  down for selling
    if (CedarHelper::isStock(req.code())) {

      leftQty = CedarHelper::stockQtyRoundDown(leftQty);
      if (leftQty == 0) {
        LOG(INFO) << "sell again with qty < 100, just recycle";
        setRecycle();
        return true;
      }
    }
  }

  req.set_trade_quantity(leftQty);

  //usually caused by reaching highlimit or lowlimit
  if (req.limit_price() == 0.0) {
    LOG(INFO) << "probably touching highlimit or lowlimit";
    setRecycle();
    return true;
  }

  service->sendRequest(getOrderReactorID(), req);
  orderState = Sending;
  return true;
}
