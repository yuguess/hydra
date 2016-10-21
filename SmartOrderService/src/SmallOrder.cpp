#include "SmallOrder.h"
#include "CedarHelper.h"
#include "CedarTimeHelper.h"
#include "MarketSpecHelper.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "SmartOrderService.h"

SmallOrder::SmallOrder(OrderRequest &req, SmartOrderService *srvc):
  OrderReactor(req, srvc), state(Init) {
  respAddr = CedarHelper::getResponseAddr();
  leftQty = req.trade_quantity();
  numOrder = 0;
  numCancel = 0;

  if (CedarHelper::isStock(req.code())) {
    lotSize = 100;
  } else {
    lotSize = 1;
  }

}

// entry functions driven by mkt update and response msg
int SmallOrder::onMsg(MessageBase &msg) {
  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mkt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);

    if (!isValidMkt(mkt))
      return 0;

    onMktUpdate(mkt);
  } else if (msg.type() == TYPE_RESPONSE_MSG) {
    ResponseMessage rsp = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
    onResponseMsg(rsp);
  }

  return 0;
}

int SmallOrder::onMktUpdate(MarketUpdate &mkt) {
  switch (state) {
    case OrderState::Init: {
      halfSpread = calHalfSpread(mkt);
      arrivalOrderPT = CedarTimeHelper::getCurPTime();
      activeLeg = Starting;

      updateLeg(SwitchSignal::Initial);
      placeOrder(mkt);

      LOG(INFO) << "snap half spread: " << halfSpread;
      logStatusInfo(mkt);
      break;
    }

    case Ready: {
      placeOrder(mkt);
      break;
    }

    //TODO: move time expiration checking to a separate timer event
    case Sent: {
      double currentMid = (mkt.bid_price(0) + mkt.ask_price(0)) * 0.5;
      double currentPrice = calPlacePrice(mkt);

      if (isMaxTimeExpired()) {
        LOG(INFO) << "max order life reached, cancelling..."; 
        cancelOrder(true);
      } else if (isTimeExpired() || isMktMoved(currentMid)) {
        LOG(INFO) << "time expired or market moved, switching legs..."; 
        LOG(INFO) << "current mid: " << currentMid;
        LOG(INFO) << "last mid: " << lastLegMid; 

        updateLeg(SwitchSignal::TimeExpired);
        cancelOrder();
      } else if (placePrice != currentPrice) {
        LOG(INFO) << "price expired, recalculating limit price...";
        cancelOrder();
      }
      break;
    }

    case TerminateCancel:
    default:
      return 0;
  }

  return 0;
}

int SmallOrder::onResponseMsg(ResponseMessage &rsp) {
  if (rsp.type() == TYPE_NEW_ORDER_CONFIRM) {
    state = Sent;
  } else if (rsp.type() == TYPE_CANCEL_ORDER_CONFIRM) {
    if (state == TerminateCancel) {
      setRecycle();
      return 0;
    }

    state = Ready;
  } else if (rsp.type() == TYPE_TRADE) {
    placeQty -= rsp.trade_quantity();
    leftQty -= rsp.trade_quantity();
    LOG(INFO) << "before trade, the place qty is: " << placeQty + rsp.trade_quantity();
    LOG(INFO) << "after trade, the place qty is: " << placeQty;
    LOG(INFO) << "after trade, the left qty is: " << leftQty;

    if (leftQty == 0) {
      setRecycle();
    } else if (leftQty < stockMinimumQty) {
      cancelOrder(true);
    } else if (placeQty == 0) {
      updateLeg(SwitchSignal::FinishPhase);
      state = Ready;
    } else if (placeQty < stockMinimumQty) {
      updateLeg(SwitchSignal::FinishPhase);
      cancelOrder();
    }

  } else if (rsp.type() == TYPE_ERROR) {
    setRecycle();
  }

  return 0;
}

int SmallOrder::updateLeg(SwitchSignal signal) {
  if (signal == SwitchSignal::Initial || signal == SwitchSignal::FinishPhase) {
    activeLeg = Passive;
  } else if (activeLeg == Passive && signal == SwitchSignal::TimeExpired) {
    activeLeg = Aggressive;
  } else if (activeLeg == Aggressive && signal == SwitchSignal::TimeExpired) {
    activeLeg = Passive; 
  }

  return 0;
}    

// functions for actually placing and cancelling orders
int SmallOrder::placeOrder(MarketUpdate &mkt) {
  if (numOrder >= maxNumOrder) {
    LOG(INFO) << "number of split orders exceeded max threshold";
    setRecycle();
    return 0;
  } 

  placePrice = calPlacePrice(mkt);
  placeQty = calPlaceQty(mkt);  

  OrderRequest child = orderRequest;
  outOrderId = CedarHelper::getOrderId();
  child.set_id(outOrderId);
  child.set_response_address(respAddr);
  child.set_type(TYPE_LIMIT_ORDER_REQUEST);
  child.set_limit_price(placePrice);
  child.set_trade_quantity(placeQty);

  LOG(INFO) << "bidPrice: " << mkt.bid_price(0);
  LOG(INFO) << "bidVol(lot): " << mkt.bid_volume(0);
  LOG(INFO) << "askPrice: " << mkt.ask_price(0);
  LOG(INFO) << "askVol(lot): " << mkt.ask_volume(0);
  LOG(INFO) << "place price: " << placePrice;
  LOG(INFO) << "place qty: " << placeQty;
  LOG(INFO) << "left qty: " << leftQty;
  LOG(INFO) << "number of placed orders: " << numOrder;
  LOG(INFO) << "number of cancelled orders: " << numCancel;
  LOG(INFO) << "working leg: " << activeLeg;
  LOG(INFO) << "last leg: " << lastLeg;
  LOG(INFO) << "current mid: " << (mkt.bid_price(0) + mkt.ask_price(0)) * 0.5;
  LOG(INFO) << "last mid: " << lastLegMid; 

  if (activeLeg != lastLeg) {
    lastLegPT = CedarTimeHelper::getCurPTime();
    lastLegMid = (mkt.bid_price(0) + mkt.ask_price(0)) * 0.5;
  }
  lastLeg = activeLeg;

  if (!isValidOrder(child)) {
    LOG(INFO) << "invalid order fields";
    return 0;
  } 

  state = Sending;
  service->sendRequest(getOrderReactorID(), child);

  numOrder++;
  LOG(INFO) << "number of placed orders: " << numOrder;

  return 0;
}

int SmallOrder::cancelOrder(bool lastCancel) {
  if (state == Canceling) {
    LOG(INFO) << "In canceling process, no need to cancel again";
    return -1;
  }

  OrderRequest child= orderRequest;
  child.set_id(CedarHelper::getOrderId());
  child.set_response_address(respAddr);
  child.set_type(TYPE_CANCEL_ORDER_REQUEST);
  child.set_cancel_order_id(outOrderId);
  child.set_trade_quantity(placeQty);
  
  state = lastCancel ? TerminateCancel : Canceling;
  service->sendRequest(getOrderReactorID(), child);
  numCancel++;
  return 0;
}

// helper funtions for checking data sanity and order status
// TODO: the following functions should be moved down to the base class
bool SmallOrder::isValidMkt(MarketUpdate &mkt) {
  if (mkt.ask_volume(0) <= 0 || mkt.bid_volume(0) <= 0 ||
      mkt.ask_price(0) <= 0.0 || mkt.bid_price(0) <= 0.0 ||
      mkt.ask_price(0) <= mkt.bid_price(0)) {
    return false;
  }

  return true;
}

bool SmallOrder::isValidOrder(OrderRequest &req) { 
  if (req.limit_price() <= 0.0 ||
      req.trade_quantity() <= 0 ||
      req.trade_quantity() % lotSize) {
    return false;
  }

  return true;
}

bool SmallOrder::isMaxTimeExpired() {
  boost::posix_time::ptime nowPT = CedarTimeHelper::getCurPTime();
  boost::posix_time::time_duration elapse = nowPT - arrivalOrderPT;
  int elapseSeconds = elapse.total_seconds();

  // take out the break session if order life stretchs over it
  if (nowPT >= getAfternoonStart() && arrivalOrderPT <= getMorningEnd())
    elapseSeconds -= breakSessionPeriod;

  return (elapseSeconds > maxTimePeriod);
}

bool SmallOrder::isMktMoved(double currentMid) {
  if (orderRequest.buy_sell() == LONG_BUY) { 
    return (currentMid - lastLegMid) > (mmHSpreadMultiple * halfSpread);
  } else { 
    return (lastLegMid - currentMid) > (mmHSpreadMultiple * halfSpread);
  }
}

bool SmallOrder::isTimeExpired() {
  boost::posix_time::ptime nowPT = CedarTimeHelper::getCurPTime();
  boost::posix_time::time_duration elapse = nowPT - lastLegPT;
  int elapseSeconds = elapse.total_seconds();

  // take out the break session if order life stretchs over it
  if (nowPT >= getAfternoonStart() && lastLegPT <= getMorningEnd())
    elapseSeconds -= breakSessionPeriod;

  srand(time(NULL));
  int delta = rand() % (2 * refreshTimeDelta) - refreshTimeDelta; 
  LOG(INFO) << "delta time of this iteration is(sec): " << delta;
  return elapseSeconds > (refreshTimePeriod + delta);
}

// functions for calculating order placement qty and price
double SmallOrder::calPlacePrice(MarketUpdate &mkt) {
  double topBid = mkt.bid_price(0);
  double topAsk = mkt.ask_price(0);

  if ((orderRequest.buy_sell() == LONG_BUY && activeLeg == OrderLeg::Passive) || 
     (orderRequest.buy_sell() == SHORT_SELL && activeLeg == OrderLeg::Aggressive)) {
    return topBid;
  }

  return topAsk;
}

int SmallOrder::calPlaceQty(MarketUpdate &mkt) {
  int qty = 0;
  int minPlaceQty = 1 * lotSize;

  int bidVol = mkt.bid_volume(0);
  int askVol = mkt.ask_volume(0);

  bidVol *= lotSize;
  askVol *= lotSize;

  if ((orderRequest.buy_sell() == LONG_BUY && activeLeg == OrderLeg::Passive) || 
     (orderRequest.buy_sell() == SHORT_SELL && activeLeg == OrderLeg::Aggressive)) {
    qty = std::max(bidVol * touchSizePercent / 100, minPlaceQty);
    qty = std::min(qty, leftQty);
  } else {
    qty = std::max(askVol * touchSizePercent / 100, minPlaceQty);
    qty = std::min(qty, leftQty);
  }  
  
  if (CedarHelper::isStock(orderRequest.code())) {
    qty = CedarHelper::stockQtyRoundDown(qty);
  }

  return qty;
}

double SmallOrder::calHalfSpread(MarketUpdate &mkt) {
  double topBid = mkt.bid_price(0);
  double topAsk = mkt.ask_price(0);
  return (topAsk - topBid) * 0.5;
}
