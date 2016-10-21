#ifndef ORDER_AGENT_H
#define ORDER_AGENT_H

#include "Backtester.h"
#include "ProtoBufMsgHub.h"
#include "EnumStringMap.h"

class OrderAgent {
public:
  OrderAgent(Backtester &bt, ProtoBufMsgHub &msg) :
    backtester(bt), msgHub(msg) {

    std::string modeStr;
    //Backtest, Livetest, LiveTrading
    CedarJsonConfig::getInstance().getStringByPath("Strategy.Mode", modeStr);
    mode = StringToEnum::toStrategyMode(modeStr);

    if (mode == LIVE_TRADING) {
      CedarHelper::setupTradeServerMap(accToAddr);
    }
  }

  int sendRequest(OrderRequest &req) {
    switch (mode) {
      case BACKTEST:
        LOG(INFO) << "order to backtester";
        //goes to backtester
        //backtester.sendRequest();
        break;
      case LIVETEST:
        LOG(INFO) << "order to virtual matching engine";
        break;
        //send through orderAgent
      case LIVE_TRADING:
        LOG(INFO) << "order request to liveTrading";
        dispatchOrder(req);
      default:
        LOG(FATAL) << "Invalid Strategy Mode " << mode;
    }

    return 0;
  }

private:
  bool dispatchOrder(OrderRequest &req) {
    if (req.type() == TYPE_SMART_ORDER_REQUEST ||
      req.type() == TYPE_FIRST_LEVEL_ORDER_REQUEST) {

      if (accToAddr.find("SmartOrderService") == accToAddr.end()) {
        LOG(ERROR) << "Can't find SmartOrderService";
        return false;
      }

      msgHub.pushMsg(accToAddr["SmartOrderService"],
        ProtoBufHelper::wrapMsg(TYPE_ORDER_REQUEST, req));

      return true;

    } else if (req.type() == TYPE_LIMIT_ORDER_REQUEST ||
      req.type() == TYPE_CANCEL_ORDER_REQUEST) {

      if (accToAddr.find("SmartOrderService") == accToAddr.end()) {
        LOG(ERROR) << "Can't find SmartOrderService";
        return false;
      }

      msgHub.pushMsg(accToAddr[req.account()],
        ProtoBufHelper::wrapMsg(TYPE_ORDER_REQUEST, req));
      return false;
    }

    LOG(ERROR) << "Unsupport request type " << req.type();
    return false;
  }

  StrategyMode mode;
  Backtester &backtester;
  ProtoBufMsgHub &msgHub;
  std::map<std::string, std::string> accToAddr;
};

#endif
