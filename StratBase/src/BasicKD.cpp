#include <iostream>
#include "CedarHelper.h"
#include "ProtoBufMsgHub.h"
#include "BasicKD.h"

BasicKD::BasicKD() : fiveMinStat(10, "09:15:00", "15:00:00") {
}

int BasicKD::onMsg(MessageBase &msg) {
  if (msg.type() == TYPE_MARKETUPDATE) {
    RangeStatResult rangeStat;
    MarketUpdate mkt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    LOG(INFO) << mkt.DebugString();
    getchar();
    if (!fiveMinStat.onTickUpdate(mkt, rangeStat)) {
      return 0;
    }

    LOG(INFO) << "open " << rangeStat.open;
    LOG(INFO) << "high " << rangeStat.high;
    LOG(INFO) << "low " << rangeStat.low;
    LOG(INFO) << "close " << rangeStat.close;

    //KD.update(rangeStat.high, rangeStat.low, rangeStat.close, resK, resD);
    //k, d = self.kd.update(data.high, data.low, data.close)
    //orderDelegate.onTickUpdate(mktUpdt);
    //twoMin.onTickUpdate(mktUpdt);
    //positionManager.onTickUpdate(mktUpdt);
  } else if (msg.type() == TYPE_RESPONSE_MSG) {
    ResponseMessage respMsg = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
    //update orderDelegate
    //orderDelegate.onOrderResponseUpdate(respMsg);
    //positionManager.onOrderResponseUpdate(respMsg);
  }
  //check stop profit/loss on every tick
  return 0;
}

bool BasicKD::onCreate() {
  if (getStrategyMode() == LIVETEST || getStrategyMode() == LIVE_TRADING) {
    PersistentState::load(jsonState);
  }

  return true;
}

bool BasicKD::onExit() {
  if (getStrategyMode() == LIVETEST || getStrategyMode() == LIVE_TRADING) {
    PersistentState::save(jsonState);
  }

  return true;
}
