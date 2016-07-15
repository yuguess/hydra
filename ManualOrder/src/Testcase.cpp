#include <iostream>
#include <vector>

#include "CedarJsonConfig.h"
#include "CedarHelper.h"
#include "CedarLogging.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "CedarHelper.h"

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
    for (int i = 0; i < tnames.size(); i++) {
      tradeServerMap[tnames[i]]= addrs[i];
    }
  }

  int onMsg(MessageBase msg) {
    CedarMsgType type = msg.type();

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

    for (int i = 0; i < ids.size(); i++) {
      OrderRequest req;
      req.set_type(TYPE_LIMIT_ORDER_REQUEST);
      req.set_id(CedarHelper::getOrderId());
      req.set_cancel_order_id(ids[i]);
      LOG(INFO) << "Cancel order " << ids[i] << std::endl;
      msgHub.pushMsg(sendAddr, 
          ProtoBufHelper::wrapMsg(TYPE_ORDER_REQUEST, req));
    }

    return 0;
  }

  int run() {
    testcase1();

    LOG(INFO) << "all sent" << std::endl;
  }

private:
  ProtoBufMsgHub msgHub;
  std::map<std::string, std::string> tradeServerMap;
};

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  CedarLogging::init("Testcase");
  CedarJsonConfig::getInstance().loadConfigFile("./config/ManualOrder.json");

  Tester tester;
  tester.run();
  return 0;
}
