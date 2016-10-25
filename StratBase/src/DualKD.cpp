#include "DualKD.h"

DualKD::DualKD() :
  qKD(13, 3, 3), sKD(13, 3, 3), rangeCollector(15 * 60, "DCE"),
  slowSignal(UNINITIALIZE), preSlowK(UNINITIALIZE), preQuickK(UNINITIALIZE) {

  respAddr = CedarHelper::getResponseAddr();
}

int DualKD::onMsg(MessageBase &msg) {

  if (msg.type() == TYPE_MARKETUPDATE) {
    LOG(INFO) << "on md update";

    MarketUpdate mkt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    RangeStat rangeStat;
    if (!rangeCollector.onRealTimeDataTickUpdate(mkt, rangeStat)) {
      return 0;
    }

    LOG(INFO) << rangeStat.DebugString();

    onRangeStatUpdate(rangeStat);

  } else if (msg.type() == TYPE_RESPONSE_MSG) {
    ResponseMessage rsp = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);

    jsonState["position"]["code"] = rsp.code();
    jsonState["position"]["price"] = rsp.price();
    jsonState["position"]["qty"] = jsonState["position"]["qty"].asInt() +
      (rsp.buy_sell() == LONG_BUY ?
      rsp.trade_quantity() : -(rsp.trade_quantity()));
    jsonState["position"]["ts"] = CedarTimeHelper::getCurTimeStamp();

  } else if (msg.type() == TYPE_RANGE_STAT) {
    RangeStat rangeStat = ProtoBufHelper::unwrapMsg<RangeStat>(msg);
    onRangeStatUpdate(rangeStat);
    LOG(INFO) << rangeStat.DebugString();
  }

  return 0;
}

bool DualKD::onRangeStatUpdate(RangeStat &range) {

  if (range.stream() == "15minData") {
    double k = 0.0, d = 0.0;
    if (!qKD.update(range.high(), range.low(), range.close(), k, d)) {
      return 0;
    }

    if (preQuickK != UNINITIALIZE && slowSignal != UNINITIALIZE) {
      //LOG(INFO) << "preK " << preQuickK << " K " << k
        //<< " slowSignal " << slowSignal;
      //getchar();

      if ((preQuickK < 0.3 && 0.3 < k) ||
        ((preQuickK < 0.7 && 0.7 < k) && slowSignal == 1)) {

        //LOG(INFO) << "Buy " << range.close() << " " << range.timestamp();
        enterMarket(LONG_BUY, range.code(), range.close(), range.timestamp());
      } else if ((preQuickK > 0.3 && 0.3 > k) ||
        ((preQuickK > 0.7 && 0.7 > k) && slowSignal == -1)) {

        //LOG(INFO) << "Sell " << range.close() << " " << range.timestamp();
        enterMarket(SHORT_SELL, range.code(), range.close(), range.timestamp());
      }
    }

    preQuickK = k;

  } else if (range.stream() == "DayData") {
    onDayUpdate(range);
  }

  return true;
}

bool DualKD::onDayUpdate(RangeStat &range) {
  double k = 0.0, d = 0.0;

  if (!sKD.update(range.high(), range.low(), range.close(), k, d)) {
    return 0;
  }

  if (preSlowK != UNINITIALIZE) {
    if ((preSlowK < 0.3 && 0.3 < k) || (preSlowK < 0.7 && 0.7 < k))
      slowSignal = SLOW_BUY;
    else if ((preSlowK > 0.3 && 0.3 > k) || (preSlowK > 0.7 && 0.7 > k))
      slowSignal = SLOW_SELL;
  }

  preSlowK = k;

  return true;
}

bool DualKD::enterMarket(TradeDirection buySell, std::string code,
  double price, std::string ts) {
  switch (getStrategyMode()) {
    case BACKTEST: {
      if (!jsonState.isMember("position") ||
        (jsonState.isMember("position") &&
         jsonState["position"]["qty"].asInt() == 0)) {

        transacLogger.enter(buySell, code, 1, price, ts);
        return true;
      }

      if (jsonState["position"]["qty"] == 1 && buySell == SHORT_SELL) {
        transacLogger.enter(buySell, code, 2, price, ts);
      } else if (jsonState["position"]["qty"] == -1 && buySell == LONG_BUY) {
        transacLogger.enter(buySell, code, 2, price, ts);
      }

      break;
    }

    case LIVETEST: {
      int qty = jsonState["position"]["qty"].asInt();
      if (!jsonState.isMember("position") ||
        (jsonState.isMember("position") && qty == 0)) {

        transacLogger.enter(buySell, code, 1, price, ts);

        jsonState["position"]["code"] = code;
        jsonState["position"]["price"] = price;
        jsonState["position"]["qty"] = (buySell == LONG_BUY ? 1 : -1);
        jsonState["position"]["ts"] = ts;
        return true;
      }

      if ((qty == 1 && buySell == SHORT_SELL) ||
        (qty == -1 && buySell == LONG_BUY)) {

        transacLogger.enter(buySell, code, 2, price, ts);

        jsonState["position"]["code"] = code;
        jsonState["position"]["price"] = price;
        jsonState["position"]["qty"] = (buySell == LONG_BUY ? 1 : -1);
        jsonState["position"]["ts"] = ts;
      }

      break;
    }

    case LIVE_TRADING: {
      int qty = jsonState["position"]["qty"].asInt();

      if (!jsonState.isMember("position") ||
        (jsonState.isMember("position") && qty == 0)) {

        OrderRequest req = CedarHelper::getInitOrderRequest(
          respAddr, TYPE_SMART_ORDER_REQUEST);

        //req.set_account();
        req.set_buy_sell(buySell);
        req.set_code(code);
        req.set_trade_quantity(1);

        orderAgent.sendRequest(req);

        return true;
      }

      if ((qty == 1 && buySell == SHORT_SELL) ||
        (qty == -1 && buySell == LONG_BUY)) {

        OrderRequest req = CedarHelper::getInitOrderRequest(
          respAddr, TYPE_SMART_ORDER_REQUEST);

        req.set_buy_sell(buySell);
        req.set_code(code);
        req.set_trade_quantity(2);

        //transacLogger.enter(buySell, code, 2, price, ts);
        orderAgent.sendRequest(req);
      }

      break;
    }

    default:
      break;
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
  LOG(INFO) << "dualKD onExit()";
  PersistentState::save(jsonState);

  if (getStrategyMode() == BACKTEST) {
    transacLogger.saveToFile();
  }

  return true;
}
