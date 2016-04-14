#include <ta-lib/ta_libc.h>
#include "StratTemplate.h"
#include "TechnicalHelper.h"

StratTemplate::StratTemplate() {
  twoMin.init(2 * 60);
  twoMin.registerCallback(std::bind(&StratTemplate::twoMinUpdate, this,
    std::placeholders::_1));
}

int StratTemplate::onMsg(MessageBase &msg) {

  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mktUpdt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    twoMin.onTickUpdate(mktUpdt);
  } else if (msg.type() == TYPE_RESPONSE_MSG) {

  }

  //updatePNL
  //check stop loss/profit on every tick

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

  TA_MA(tickIdx, tickIdx, quickMAInput, quickMAPeriod,
      TA_MAType_SMA, &outBeg, &outNbElement, quickMAOutput);
  quickMARes.push_back(quickMAOutput[0]);

  if (tickIdx < slowMAPeriod - 1) {
    slowMAInput[tickIdx] = rng.close;
    return 0;
  }

  TA_MA(tickIdx, tickIdx, slowMAInput, slowMAPeriod,
      TA_MAType_SMA, &outBeg, &outNbElement, slowMAOutput);
  slowMARes.push_back(slowMAOutput[0]);

  int quickEndIdx = quickMARes.size() - 1;
  int slowEndIdx = slowMARes.size() - 1;

  if (slowEndIdx < 1)
    return 0;

  double firBeg = quickMARes[quickEndIdx - 1];
  double firEnd = slowMARes[slowEndIdx - 1];
  double secBeg = quickMARes[quickEndIdx];
  double secEnd = slowMARes[slowEndIdx];

  if (TechnicalHelper::checkCross(firBeg, firEnd, secBeg, secEnd) == NO_CROSS) {
    return 0;
  } else if (TechnicalHelper::checkCross(
        firBeg, firEnd, secBeg, secEnd) == UP_CROSS) {

    if (positionManager.getPosition() == PositionManager::LONG_POSITION) {
      LOG(ERROR) << "";
    } else if (positionManager.getPosition() == PositionManager::SHORT_POSITION) {

      ////flat short
      //OrderRequest req;
      //req.set_type(TYPE_AGENT_ORDER_REQUEST);
      ////req.set_id();
      //req.set_code();
      ////req.set_exchange();
      ////req.set_buy_sell;
      ////req.set_limit_price;
      ////req.set_trade_quantity;
      ////req.set_open_close;


    } else if (positionManager.getPosition() == PositionManager::EMPTY) {
      //enter long
    }

  } else if (TechnicalHelper::checkCross(
        firBeg, firEnd, secBeg, secEnd) == DOWN_CROSS) {

    if (positionManager.getPosition() == PositionManager::LONG_POSITION) {
      //flat
    } else if (positionManager.getPosition() == PositionManager::SHORT_POSITION) {
      LOG(ERROR) << "";
    } else if (positionManager.getPosition() == PositionManager::EMPTY) {
      //enter short
    }
  }
}
