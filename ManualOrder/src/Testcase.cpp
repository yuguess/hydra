#include <iostream>
#include <vector>

#include "CedarJsonConfig.h"
#include "CedarHelper.h"
#include "CedarLogger.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "CedarHelper.h"
#include "IncludeOnlyInMain.h"

class Tester {
public:
  Tester() {
    ProtoBufHelper::setupProtoBufMsgHub(msgHub);
    msgHub.registerCallback(std::bind(&Tester::onMsg, this,
        std::placeholders::_1));

    std::vector<std::string> tnames, addrs;
    CedarJsonConfig::getInstance().getStringArrayWithTag(tnames, "TradeServer",
      "name");
    CedarJsonConfig::getInstance().getStringArrayWithTag(addrs, "TradeServer",
      "address");
    for (unsigned i = 0; i < tnames.size(); i++) {
      tradeServerMap[tnames[i]]= addrs[i];
    }

    respAddr = CedarHelper::getResponseAddr();
  }

  int onMsg(MessageBase msg) {
    if (msg.type() == TYPE_MARKETUPDATE) {
      MarketUpdate mkt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
      LOG(INFO) << mkt.DebugString();
    } if (msg.type() == TYPE_ORDER_REQUEST) {
      OrderRequest req = ProtoBufHelper::unwrapMsg<OrderRequest>(msg);
      LOG(INFO) << req.DebugString();
    } if (msg.type() == TYPE_RESPONSE_MSG) {
      ResponseMessage rmsg = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
      LOG(INFO) << rmsg.DebugString();
    }

    return 0;
  }

  int testcase1() {
    //only place limit order
    int numOrder = 100;
    int limitPrice = 2.859;
    std::string stkCode = "518880";
    std::string tradeServer = "2001_Stock";

    static std::string respAddr = CedarHelper::getResponseAddr();
    std::string sendAddr = tradeServerMap[tradeServer];

    for (int i = 0; i < numOrder; i++) {
      OrderRequest req;
      req.set_response_address(respAddr);
      req.set_id(CedarHelper::getOrderId());

      req.set_code(stkCode);
      req.set_type(TYPE_LIMIT_ORDER_REQUEST);
      req.set_limit_price(limitPrice);
      req.set_exchange(SHSE);
      req.set_buy_sell(LONG_BUY);
      req.set_trade_quantity(100);

      LOG(INFO) << "order " << i << std::endl;
      msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_ORDER_REQUEST, req));
    }
  }

  int testcase2() {
    //only place limit order
    int numOrder = 100;
    int limitPrice = 2.859;
    std::string stkCode = "518880";
    std::string tradeServer = "2001_Stock";

    static std::string respAddr = CedarHelper::getResponseAddr();
    std::string sendAddr = tradeServerMap[tradeServer];
    std::vector<std::string> ids;
    for (int i = 0; i < numOrder; i++) {
      OrderRequest req;
      req.set_response_address(respAddr);
      ids.push_back(CedarHelper::getOrderId());
      req.set_id(ids[i]);

      req.set_code(stkCode);
      req.set_type(TYPE_LIMIT_ORDER_REQUEST);
      req.set_limit_price(limitPrice);
      req.set_exchange(SHSE);
      req.set_buy_sell(LONG_BUY);
      req.set_trade_quantity(100);

      LOG(INFO) << "order " << i << std::endl;
      msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_ORDER_REQUEST, req));
    }

    for (unsigned i = 0; i < ids.size(); i++) {
      OrderRequest req;
      req.set_type(TYPE_CANCEL_ORDER_REQUEST);
      req.set_id(CedarHelper::getOrderId());
      req.set_cancel_order_id(ids[i]);
      LOG(INFO) << "Cancel order " << ids[i] << std::endl;
      msgHub.pushMsg(sendAddr,
          ProtoBufHelper::wrapMsg(TYPE_ORDER_REQUEST, req));
    }

    return 0;
  }

  int testFuturesToMultiAccount() {
    int numOrder = 1;
    int limitPrice = 3799;
    std::string code = "jd1609";
    std::vector<std::string> des ={"1002_Futures",
      "1005_Futures","3002_Futures"};

    OrderRequest req;
    req.set_response_address(respAddr);
    std::string id = CedarHelper::getOrderId();
    req.set_id(id);
    req.set_code(code);
    req.set_type(TYPE_LIMIT_ORDER_REQUEST);
    req.set_limit_price(limitPrice);
    req.set_buy_sell(LONG_BUY);
    req.set_trade_quantity(1);
    req.set_open_close(PositionDirection::OPEN_POSITION);

    for (unsigned i = 0; i < des.size(); i++) {
      LOG(INFO) << "send order " << tradeServerMap[des[i]];
      msgHub.pushMsg(tradeServerMap[des[i]],
          ProtoBufHelper::wrapMsg(TYPE_ORDER_REQUEST, req));
    }

    sleep(5);

    OrderRequest cncl;
    cncl.set_type(TYPE_CANCEL_ORDER_REQUEST);
    cncl.set_id(CedarHelper::getOrderId());
    cncl.set_cancel_order_id(id);

    for (unsigned i = 0; i < des.size(); i++) {
      LOG(INFO) << "cancel order " << tradeServerMap[des[i]];
      msgHub.pushMsg(tradeServerMap[des[i]],
        ProtoBufHelper::wrapMsg(TYPE_ORDER_REQUEST, cncl));
    }

    return 0;
  }

  int testToMultiAccount() {
    return 0;
  }

  int run() {
    //testcase1();

    LOG(INFO) << "all sent" << std::endl;
    return 0;
  }

private:
  ProtoBufMsgHub msgHub;
  std::map<std::string, std::string> tradeServerMap;
  std::string respAddr;
};

int main(int argc, char *argv[]) {
  CedarHelper::cedarAppInit(argc, argv);

  Tester tester;
  tester.run();
  return 0;
}
