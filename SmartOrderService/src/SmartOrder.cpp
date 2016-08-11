#include "SmartOrder.h"
#include "CedarHelper.h"

SmartOrder::SmartOrder(OrderRequest &req, SmartOrderService *srvc):
  OrderReactor(req, srvc), state(Init) {

  respAddr = CedarHelper::getResponseAddr();
  leftQty = req.trade_quantity();
}

int SmartOrder::onMsg(MessageBase &msg) {
  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mkt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    //TODO filter out invalid mkt update, ensure no mkt has 0 vol
    onMktUpdate(mkt);
  } else if (msg.type() == TYPE_RESPONSE_MSG) {
    ResponseMessage rsp = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
    onResponseMsg(rsp);
  }

  return 0;
}

int SmartOrder::onMktUpdate(MarketUpdate &mkt) {
  switch (state) {
    case SmartOrderState::Init: {
      referencePrice = calReferencePrice(mkt);
      sendNewLimitOrder(mkt);
      break;
    }

    case Ready: {
      sendNewLimitOrder(mkt);
      break;
    }

    case Sent: {
      int topVol = orderRequest.buy_sell() == LONG_BUY ?
        mkt.bid_volume(0): mkt.ask_volume(0);

      if (isOnlyOneInTopLadder(topVol) || isPriceExpire()) {
        if (lastOrderPrice != calWeightPrice(mkt)) {
          LOG(INFO) << "retreat";
          cancelOrder();
          state = Canceling;
        } else {
          lastOrderTs = std::chrono::system_clock::now();
        }
      }
    }

    case TerminateCancel:
    default:
      return 0;
  }
  return 0;
}

bool SmartOrder::isPriceExpire() {
  std::chrono::seconds elapse = std::chrono::duration_cast<
    std::chrono::seconds>(std::chrono::system_clock::now()- lastOrderTs);

  return elapse.count() > refreshTimePeriod;
}

bool SmartOrder::sendNewLimitOrder(MarketUpdate &mkt) {
  outOrderId = CedarHelper::getOrderId();
  double limitPrice = calWeightPrice(mkt);

  OrderRequest req = orderRequest;
  req.set_response_address(respAddr);
  req.set_type(TYPE_LIMIT_ORDER_REQUEST);
  req.set_limit_price(limitPrice);
  req.set_id(outOrderId);
  req.set_trade_quantity(leftQty);

  state = Sending;
  service->sendRequest(getOrderReactorID(), req);

  lastOrderPrice = limitPrice;
  lastOrderTs = std::chrono::system_clock::now();
  return true;
}

bool SmartOrder::isOnlyOneInTopLadder(int topVol) {
  if (CedarHelper::isStock(orderRequest.code())) {
    topVol *= 100;
  }

  if (topVol == leftQty) {
    return true;
  }

  return false;
}

int SmartOrder::onResponseMsg(ResponseMessage &rsp) {
  if (rsp.type() == TYPE_NEW_ORDER_CONFIRM) {
    state = Sent;
  } else if (rsp.type() == TYPE_CANCEL_ORDER_CONFIRM) {
    if (state == TerminateCancel) {
      setRecycle();
      return 0;
    }

    state = Ready;
  } else if (rsp.type() == TYPE_TRADE) {
    leftQty -= rsp.trade_quantity();
    if (CedarHelper::isStock(orderRequest.code())
      && leftQty < stockMinimumQty) {

      if (leftQty == 0) {
        setRecycle();
        return 0;
      }

      cancelOrder();
      state = TerminateCancel;
    }
  } else if (rsp.type() == TYPE_ERROR) {
    setRecycle();
  }

  return 0;
}

int SmartOrder::cancelOrder() {
  OrderRequest req = orderRequest;
  req.set_id(CedarHelper::getOrderId());
  req.set_response_address(respAddr);
  req.set_type(TYPE_CANCEL_ORDER_REQUEST);
  req.set_cancel_order_id(outOrderId);
  service->sendRequest(getOrderReactorID(), req);
  return 0;
}

double SmartOrder::calReferencePrice(MarketUpdate &mkt) {
  double topBid = mkt.bid_price(0);
  double topAsk = mkt.ask_price(0);
  double bidVol = mkt.bid_volume(0);
  double askVol = mkt.ask_volume(0);
  return (topBid * askVol + topAsk * bidVol) / (askVol + bidVol);
}

double SmartOrder::calWeightPrice(MarketUpdate &mkt) {
  double bidAvgPrice = avgBidPrice(mkt);
  double askAvgPrice = avgAskPrice(mkt);
  int bidVol = totalBidVol(mkt);
  int askVol = totalAskVol(mkt);
  double weightPrice =
    (bidAvgPrice * askVol + askAvgPrice * bidVol) / (bidVol + askVol);

  LOG(INFO) << "bidAvgPrice " << bidAvgPrice;
  LOG(INFO) << "askAvgPrice " << askAvgPrice;
  LOG(INFO) << "bidVol " << bidVol;
  LOG(INFO) << "askVol " << askVol;
  LOG(INFO) << "weight " << weightPrice;

  return roundToLadder(weightPrice, mkt);
}

double SmartOrder::roundToLadder(double price, MarketUpdate &mkt) {
  double topAsk = mkt.ask_price(0);
  double topBid = mkt.bid_price(0);
  TradeDirection buySell = orderRequest.buy_sell();
  std::string code = orderRequest.code();

  if (price > topAsk) {
    return (buySell == LONG_BUY) ? topAsk : roundDownAskLadder(price, mkt);
  } else if (price < topBid) {
    return (buySell == LONG_BUY) ? roundUpBidLadder(price, mkt) : topBid;
  }

  return (buySell == LONG_BUY) ? topAsk : topBid;
  //return (buySell == LONG_BUY) ?
  //    topBid + oneTickPrice(code) : topAsk - oneTickPrice(code);
}

double SmartOrder::roundUpBidLadder(double price, MarketUpdate &mkt) {
  double roundPrice = mkt.bid_price(0);
  int idx = 1;
  while (idx < mkt.bid_price_size() && price < mkt.bid_price(idx)) {
    roundPrice = mkt.bid_price(idx);
    idx++;
  }
  return roundPrice;
}

double SmartOrder::roundDownAskLadder(double price, MarketUpdate &mkt) {
  double roundPrice = mkt.ask_price(0);
  int idx = 1;
  while (idx < mkt.ask_price_size() && price > mkt.ask_price(idx)) {
    roundPrice = mkt.ask_price(idx);
    idx++;
  }
  return roundPrice;
}

double SmartOrder::oneTickPrice(std::string &code) {
  //TODO fix this later
  return 0.01;
}

double SmartOrder::avgBidPrice(MarketUpdate &mkt) {
  double bidVol = totalBidVol(mkt);
  double avgPrice = 0.0;
  for (int i = 0; i < mkt.bid_price_size(); i++) {
    double weight = (mkt.bid_volume(i) / bidVol);
    avgPrice += mkt.bid_price(i) * weight;
  }
  return avgPrice;
}

double SmartOrder::avgAskPrice(MarketUpdate &mkt) {
  double askVol = totalAskVol(mkt);
  double avgPrice = 0.0;
  for (int i = 0; i < mkt.ask_price_size(); i++) {
    double weight = (mkt.ask_volume(i) / askVol);
    avgPrice += mkt.ask_price(i) * weight;
  }
  return avgPrice;
}

int SmartOrder::totalBidVol(MarketUpdate &mkt) {
  int bidVol = 0;
  for (int i = 0; i < mkt.bid_volume_size(); i++) {
    bidVol += mkt.bid_volume(i);
  }
  return bidVol;
}

int SmartOrder::totalAskVol(MarketUpdate &mkt) {
  int askVol = 0;
  for (int i = 0; i < mkt.ask_volume_size(); i++) {
    askVol += mkt.ask_volume(i);
  }
  return askVol;
}
