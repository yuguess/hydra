#include "StratTemplate.h"


int StratTemplate::onMsg(MessageBase &msg) {

  RangeStat fiveMin(300);
  fiveMin.registerCallback(std::bind(&StratTemplate::fiveMinUpdate,
        this, std::placeholders::_1));
  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mktUpdt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    fiveMin.onTickUpdate(mktUpdt);
  }

  //check stop loss on every tick

  return 0;
}

int StratTemplate::fiveMinUpdate(RangeStatData &rng) {

//update quick slow MA
//check cross
//emit buy or sell signal
//send orderAgent buy
//send orderAgent sell
}
