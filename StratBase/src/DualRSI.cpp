#include "DualRSI.h"

namespace pt = boost::posix_time;

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
    onRangeStatUpdate(range);
  }

  return 0;
}

bool DualRSI::onRangeStatUpdate(RangeStat &range) {
  if (range.stream() == "5minData") {
    if (!qRSI.update(range.close(), qRSIVal)) {
      return 0;
    }
  }

  if (range.stream() == "15minData") {
    if (!sRSI.update(range.close(), sRSIVal)) {
      return 0;
    }
    sRSIValValidFlag = true;
  }

  if (range.stream() == "DayData") {
    //close out all position when day end
    flatAll(range.code(), range.close(), range.timestamp());
    return 0;
  }

  if (!sRSIValValidFlag)
    return 0;

  if (isBelowTimeThre(range.timestamp()))
    return 0;

  //LOG(INFO) << "ts " << range.timestamp();
  //LOG(INFO) << "close " << range.close();
  //LOG(INFO) << "stream " << range.stream();
  LOG(INFO) << "qRSIVal " << qRSIVal;
  LOG(INFO) << "sRSIVal " << sRSIVal;

  if (qRSIVal > quickRSIThre && sRSIVal > slowRSIThre) {
    //LOG(INFO) << "qRSIVal " << qRSIVal;
    //LOG(INFO) << "sRSIVal " << sRSIVal;
    //LOG(INFO) << "quickRSIThre " << quickRSIThre;
    //LOG(INFO) << "slowRSIThre " << slowRSIThre;
    //LOG(INFO) << "Buy " << range.close() << " " << range.timestamp();
    //getchar();
    enterMarket("Buy", range.code(), range.close(), range.timestamp());
  }

  if (qRSIVal < (1 - quickRSIThre) && sRSIVal < (1 - slowRSIThre)) {
    //LOG(INFO) << "qRSIVal " << qRSIVal;
    //LOG(INFO) << "sRSIVal " << sRSIVal;
    //LOG(INFO) << "quickRSIThre " << quickRSIThre;
    //LOG(INFO) << "slowRSIThre " << slowRSIThre;
    //LOG(INFO) << "Sell " << range.close() << " " << range.timestamp();
    //getchar();
    enterMarket("Sell", range.code(), range.close(), range.timestamp());
  }

  //if data.dataStream == "5Min":
    //self.myBook.updatePNL(data.close, ts)

  //if (!qKD.update(range.high(), range.low(), range.close(), k, d))
      //return 0;
  return true;
}

bool DualRSI::flatAll(std::string code, double price, std::string ts) {
  if (curPosition == 0)
    return true;
  else if (curPosition > 0)
    transacLogger.enter("Sell", code, curPosition, price, ts);
  else
    transacLogger.enter("Buy", code, -curPosition, price, ts);

  curPosition = 0;
  jsonState.removeMember("preEnter");

  return true;
}

bool DualRSI::enterMarket(std::string buySell, std::string code,
  double price, std::string ts) {

  curPosition = (buySell == "Buy" ? 1 : -1);

  if (!jsonState.isMember("preEnter")) {
    transacLogger.enter(buySell, code, 1, price, ts);
    jsonState["preEnter"] = buySell;
    return true;
  }

  if (jsonState["preEnter"] != buySell) {
    transacLogger.enter(buySell, code, 2, price, ts);
    jsonState["preEnter"] = buySell;
  }

  return true;
}

bool DualRSI::isBelowTimeThre(std::string ts) {
  std::string hmsThre = "11:15:00";
  //YYYY-mm-dd HH:MM:SS
  std::string timeThreStr = ts.substr(0, 11) + hmsThre;
  return CedarTimeHelper::strToPTime("%Y-%m-%d %H:%M:%S", ts) <
    CedarTimeHelper::strToPTime("%Y-%m-%d %H:%M:%S", timeThreStr);
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
