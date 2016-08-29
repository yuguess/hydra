#include "DualRSI.h"

int DualRSI::onMsg(MessageBase &msg) {

  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mktUpdt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    //orderDelegate.onTickUpdate(mktUpdt);
    //twoMin.onTickUpdate(mktUpdt);
    //positionManager.onTickUpdate(mktUpdt);

  } else if (msg.type() == TYPE_RESPONSE_MSG) {
    ResponseMessage rsp = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
    ////update orderDelegate
    //orderDelegate.onOrderResponseUpdate(respMsg);
    //positionManager.onOrderResponseUpdate(respMsg);
  } else if (msg.type() == TYPE_RANGE_STAT) {
    RangeStat range = ProtoBufHelper::unwrapMsg<RangeStat>(msg);
    bool qRSIFlag = false, sRSIFlag = false;
    //if (range.stream() == ) {

    //}

    //if (range.stream() == ) {

    //}

  }

  return 0;
}

bool DualRSI::onCreate() {
  if (getStrategyMode() == LIVETEST || getStrategyMode() == LIVE_TRADING) {
    PersistentState::load(jsonState);
  }

  return true;
}

bool DualRSI::onExit() {
  if (getStrategyMode() == LIVETEST || getStrategyMode() == LIVE_TRADING) {
    PersistentState::save(jsonState);
  }

  if (getStrategyMode() == BACKTEST) {
    transacLogger.saveToFile();
  }

  return true;
}
