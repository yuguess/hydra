#include <iostream>
#include "CedarHelper.h"
#include "ProtoBufMsgHub.h"
#include "BasicKD.h"

BasicKD::BasicKD() :
  fiveMinStat(900, "09:15:00", "11:30:00", "13:00:00", "15:15:00") {
}

int BasicKD::onMsg(MessageBase &msg) {
  if (msg.type() == TYPE_MARKETUPDATE) {
    RangeStatResult rangeStat;
    MarketUpdate mkt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    //getchar();
    //LOG(INFO) << mkt.DebugString();
    if (!fiveMinStat.onTickUpdate(mkt, rangeStat)) {
      return 0;
    }

    LOG(INFO) << "open " << rangeStat.open;
    LOG(INFO) << "high " << rangeStat.high;
    LOG(INFO) << "low " << rangeStat.low;
    LOG(INFO) << "close " << rangeStat.close;
    LOG(INFO) << "count " << rangeStat.tickCount;
    LOG(INFO) << "begin " << rangeStat.start;
    LOG(INFO) << "end " << rangeStat.end;

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
  } else if (msg.type() == TYPE_RANGE_STAT) {
    RangeStat range = ProtoBufHelper::unwrapMsg<RangeStat>(msg);
    LOG(INFO) << range.DebugString();
    getchar();
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
