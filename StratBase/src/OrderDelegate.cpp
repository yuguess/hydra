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

int OrderDelegate::sendRequest(OrderRequest &req) {
  //right now, we just cross to trade
  return 0;
}
