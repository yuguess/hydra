#include "DualKD.h"

int DualKD::onMsg(MessageBase &msg) {

  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mktUpdt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);

    //orderDelegate.onTickUpdate(mktUpdt);
    //twoMin.onTickUpdate(mktUpdt);
    //positionManager.onTickUpdate(mktUpdt);

  } else if (msg.type() == TYPE_RESPONSE_MSG) {
    ResponseMessage rsp = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
    //update orderDelegate
    //orderDelegate.onOrderResponseUpdate(respMsg);
    //positionManager.onOrderResponseUpdate(respMsg);

  } else if (msg.type() == TYPE_RANGE_STAT) {
    RangeStat range = ProtoBufHelper::unwrapMsg<RangeStat>(msg);

    //LOG(INFO) << "stream " << range.stream();
    //LOG(INFO) << "ts " << range.timestamp();
    //LOG(INFO) << "high " << range.high();
    //LOG(INFO) << "low " << range.low();
    //LOG(INFO) << "close " << range.close();

    if (range.stream() == "15minData") {

      double k = 0.0, d = 0.0;
      if (!qKD.update(range.high(), range.low(), range.close(), k, d))
        return 0;

      if (jsonState.isMember("preQuickK") && jsonState.isMember("slowSignal")) {
        double preQuickK = jsonState["preQuickK"].asDouble();
        int slowSignal = jsonState["slowSignal"].asInt();

        //LOG(INFO) << "preK " << preQuickK << " K " << k << " slowSignal "
        //  << slowSignal;
        //getchar();

        if ((preQuickK < 0.3 && 0.3 < k) ||
          ((preQuickK < 0.7 && 0.7 < k) && slowSignal == 1)) {

          //LOG(INFO) << "Buy " << range.close() << " " << range.timestamp();
          enterMarket("Buy", range.code(), range.close(), range.timestamp());
        } else if ((preQuickK > 0.3 && 0.3 > k) ||
          ((preQuickK > 0.7 && 0.7 > k) && slowSignal == -1)) {

          //LOG(INFO) << "Sell " << range.close() << " " << range.timestamp();
          enterMarket("Sell", range.code(), range.close(), range.timestamp());
        }
      }

      jsonState["preQuickK"] = k;
      jsonState["preQuickD"] = d;

    } else if (range.stream() == "DayData") {

      double k = 0.0, d = 0.0;
      if (!sKD.update(range.high(), range.low(), range.close(), k, d))
        return 0;

      if (jsonState.isMember("preSlowK")) {
        double preSlowK = jsonState["preSlowK"].asDouble();

        if ((preSlowK < 0.3 && 0.3 < k) || (preSlowK < 0.7 && 0.7 < k))
          jsonState["slowSignal"] = 1;
        else if ((preSlowK > 0.3 && 0.3 > k) || (preSlowK > 0.7 && 0.7 > k))
          jsonState["slowSignal"] = -1;
      }

      jsonState["preSlowK"] = k;
      jsonState["preSlowD"] = d;
    }
  }

  return 0;
}

bool DualKD::enterMarket(std::string buySell, std::string code,
  double price, std::string ts) {

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

bool DualKD::onCreate() {
  if (getStrategyMode() == LIVETEST || getStrategyMode() == LIVE_TRADING) {
    PersistentState::load(jsonState);
  }

  return true;
}

bool DualKD::onExit() {
  if (getStrategyMode() == LIVETEST || getStrategyMode() == LIVE_TRADING) {
    PersistentState::save(jsonState);
  }

  LOG(INFO) << "dualKD onExit()";

  if (getStrategyMode() == BACKTEST) {
    transacLogger.saveToFile();
  }

  return true;
}
