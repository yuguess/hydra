#include <ta-lib/ta_libc.h>
#include "StratTemplate.h"
#include "TechnicalHelper.h"
#include "EnumStringMap.h"

StratTemplate::StratTemplate() :
  orderDelegate(std::shared_ptr<StratBase>(this)) {

  std::vector<std::string> codes;
  std::vector<std::string> argLists;
  CedarJsonConfig::getInstance().getStringArrayWithTag(codes,
      "Ticker", "Code");
  CedarJsonConfig::getInstance().getStringArrayWithTag(codes,
      "Ticker", "Arglist");
  tradeSecurity = codes[0];
  exchange = argLists[0];

  twoMin.init(2 * 60);
  twoMin.registerCallback(std::bind(&StratTemplate::twoMinUpdate, this,
    std::placeholders::_1));

}

int StratTemplate::onMsg(MessageBase &msg) {

  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mktUpdt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    //orderDelegate.onTickUpdate(mktUpdt);
    //twoMin.onTickUpdate(mktUpdt);
    //positionManager.onTickUpdate(mktUpdt);

  } else if (msg.type() == TYPE_RESPONSE_MSG) {
    //ResponseMessage respMsg = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
    ////update orderDelegate
    //orderDelegate.onOrderResponseUpdate(respMsg);
    //positionManager.onOrderResponseUpdate(respMsg);
  }

  //check stop profit/loss on every tick

  return 0;
}

int StratTemplate::twoMinUpdate(RangeStatData &rng) {
  static int tickIdx = -1;
  static double slowMAInput[15000];
  static double quickMAInput[15000];
  static double slowMAOutput[10];
  static double quickMAOutput[10];
  static int slowMAPeriod = 5;
  static int quickMAPeriod = 20;
  static int outBeg, outNbElement;
  static std::vector<double> quickMARes, slowMARes;
  tickIdx++;

  if (tickIdx < quickMAPeriod - 1) {
    quickMAInput[tickIdx] = rng.close;
    return 0;
  }

  //TA_MA(tickIdx, tickIdx, quickMAInput, quickMAPeriod,
      //TA_MAType_SMA, &outBeg, &outNbElement, quickMAOutput);
  quickMARes.push_back(quickMAOutput[0]);

  if (tickIdx < slowMAPeriod - 1) {
    slowMAInput[tickIdx] = rng.close;
    return 0;
  }

  //TA_MA(tickIdx, tickIdx, slowMAInput, slowMAPeriod,
      //TA_MAType_SMA, &outBeg, &outNbElement, slowMAOutput);
  slowMARes.push_back(slowMAOutput[0]);

  int quickEndIdx = quickMARes.size() - 1;
  int slowEndIdx = slowMARes.size() - 1;

  if (slowEndIdx < 1)
    return 0;

  double firBeg = quickMARes[quickEndIdx - 1];
  double firEnd = slowMARes[slowEndIdx - 1];
  double secBeg = quickMARes[quickEndIdx];
  double secEnd = slowMARes[slowEndIdx];

  ItemPosition pos  = positionManager.getPosition().begin()->second;
  if (TechnicalHelper::checkCross(firBeg, firEnd, secBeg, secEnd) == NO_CROSS) {
    return 0;
  } else if (TechnicalHelper::checkCross(
        firBeg, firEnd, secBeg, secEnd) == UP_CROSS) {

    if (pos.netPosition > 0) {
      LOG(ERROR) << "";
    } else {
      OrderRequest req;
      //req.set_type(TYPE_DELEGATE_ORDER_REQUEST);
      req.set_exchange(StringToEnum::toExchangeType(exchange));
      req.set_code(tradeSecurity);
      req.set_trade_quantity(1);
      req.set_buy_sell(LONG_BUY);

      if (pos.netPosition < 0) {
        //flat pre short
        req.set_open_close(CLOSE_POSITION);
      } else if (pos.netPosition == 0) {
        req.set_open_close(OPEN_POSITION);
      }
      orderDelegate.sendRequest(req);
    }

  } else if (TechnicalHelper::checkCross(
        firBeg, firEnd, secBeg, secEnd) == DOWN_CROSS) {

    if (pos.netPosition < 0) {
      LOG(ERROR) << "";
    } else {
      OrderRequest req;
      //req.set_type(TYPE_DELEGATE_ORDER_REQUEST);
      req.set_exchange(StringToEnum::toExchangeType(exchange));
      req.set_code(tradeSecurity);
      req.set_trade_quantity(1);
      req.set_buy_sell(SHORT_SELL);

      if (pos.netPosition > 0) {
        //flat pre long
        req.set_open_close(CLOSE_POSITION);
      } else if (pos.netPosition == 0) {
        req.set_open_close(OPEN_POSITION);
      }
    }
  }
}
