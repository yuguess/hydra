#include "OrderDelegate.h"

int OrderDelegate::onTickUpdate(MarketUpdate &mkt) {
  if (dataBufs.find(mkt.code()) == dataBufs.end()) {
    std::vector<MarketUpdate> buf;
    buf.push_back(mkt);
    dataBufs[mkt.code()] = buf;
  } else {
    dataBufs[mkt.code()].push_back(mkt);
  }

  return 0;
}

int OrderDelegate::onOrderResponseUpdate(ResponseMessage &respMsg) {

  return 0;
}

int OrderDelegate::sendRequest(OrderRequest &req) {
  static std::string respAddr = CedarHelper::getResponseAddr();

  //right now, we just cross to trade
  if (dataBufs.find(req.code()) == dataBufs.end()) {
    LOG(ERROR) << "data order delegate doesn't have tick data, check your"
                << " order request";
    return -1;
  }

  MarketUpdate lastTick = dataBufs[req.code()].back();

  req.set_response_address(respAddr);
  req.set_id(getOrderIdString());

  if (req.buy_sell() == LONG_BUY) {
    req.set_limit_price(lastTick.ask_price(0));
  } else if (req.buy_sell() == SHORT_SELL) {
    req.set_limit_price(lastTick.bid_price(0));
  }

  return 0;
}
