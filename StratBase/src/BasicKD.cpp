#include <iostream>
#include "CedarHelper.h"
#include "ProtoBufMsgHub.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "BasicKD.h"

BasicKD::BasicKD() : kd(14, 3, 3),  fiveMinData(60, "SHSE") {
  respAddr = CedarHelper::getResponseAddr();
}

int BasicKD::onMsg(MessageBase &msg) {
  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mkt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    RangeStat rangeStat;
    if (!fiveMinData.onRealTimeDataTickUpdate(mkt, rangeStat)) {
      return 0;
    }

    LOG(INFO) << "open " << rangeStat.open();
    LOG(INFO) << "high " << rangeStat.high();
    LOG(INFO) << "low " << rangeStat.low();
    LOG(INFO) << "close " << rangeStat.close();
    LOG(INFO) << "ts " << rangeStat.timestamp();
    LOG(INFO) << "begin " << rangeStat.begin_timestamp();
    LOG(INFO) << "end" << rangeStat.end_timestamp();

    onRangeStatUpdate(rangeStat);

  } else if (msg.type() == TYPE_RESPONSE_MSG) {
    ResponseMessage respMsg = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
    //update orderDelegate
    //orderDelegate.onOrderResponseUpdate(respMsg);
    //positionManager.onOrderResponseUpdate(respMsg);
  } else if (msg.type() == TYPE_RANGE_STAT) {

    RangeStat range = ProtoBufHelper::unwrapMsg<RangeStat>(msg);

    //LOG(INFO) << "open " << range.open();
    //LOG(INFO) << "high " << range.high();
    //LOG(INFO) << "low " << range.low();
    //LOG(INFO) << "close " << range.close();

    onRangeStatUpdate(range);
  }

  return 0;
}

int BasicKD::onRangeStatUpdate(RangeStat &range) {
  double k = 0.0, d = 0.0;
  int qty = 0;

  if (!kd.update(range.high(), range.low(), range.close(), k, d))
    return 0;

  if (jsonState.isMember("preK")) {
    double preK = jsonState["preK"].asDouble();
    //LOG(INFO) << "preK " << preK << " K " << k;
    if ((preK < 0.3 && 0.3 < k) || (preK < 0.7 && 0.7 < k)) {
      LOG(INFO) << "long " << range.close();

      if (!jsonState.isMember("preEnter")) {
        qty = 1;
        enterMarket("Buy", range.code(), qty, range.close(),
          range.timestamp());
        return 0;
      }

      if (jsonState["preEnter"] != "Buy") {
        qty = 2;
        enterMarket("Buy", range.code(), qty, range.close(),
          range.timestamp());
      }

    } else if ((preK > 0.3 && 0.3 > k) || (preK > 0.7 && 0.7 > k)) {

      LOG(INFO) << "short " << range.close();

      if (!jsonState.isMember("preEnter")) {
        qty = 1;
        enterMarket("Sell", range.code(), qty, range.close(),
          range.timestamp());
        return 0;
      }

      if (jsonState["preEnter"] != "Sell") {
        qty = 2;
        enterMarket("Sell", range.code(), qty, range.close(),
          range.timestamp());
      }
    }
  }

  jsonState["preK"] = k;
  jsonState["preD"] = d;

  //LOG(INFO) << range.DebugString();
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
    //update day data

    PersistentState::save(jsonState);
  }

  if (getStrategyMode() == BACKTEST) {
    transacLogger.saveToFile();
  }

  return true;
}

bool BasicKD::enterMarket(std::string dir, std::string code, int qty,
  double targetPrice, std::string ts) {

  if (getStrategyMode() == BACKTEST || getStrategyMode() == LIVETEST) {
    transacLogger.enter(dir, code, qty, targetPrice, ts);
  } else if (getStrategyMode() == LIVE_TRADING) {
    OrderRequest req = getInitOrderRequest();
    //req.set_code();
    req.set_type(TYPE_SMART_ORDER_REQUEST);
    dir == "Buy" ?  req.set_buy_sell(LONG_BUY) : req.set_buy_sell(SHORT_SELL);
    req.set_trade_quantity(qty);
    orderAgent.sendRequest(req);
  }

  jsonState["preEnter"] = dir;

  return true;
}

OrderRequest BasicKD::getInitOrderRequest() {
  OrderRequest req;
  std::string outOrderId = CedarHelper::getOrderId();

  req.set_response_address(respAddr);
  req.set_type(TYPE_SMART_ORDER_REQUEST);
  req.set_id(outOrderId);

  return req;
}
