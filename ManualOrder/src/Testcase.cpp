#include <iostream>
#include <vector>

#include "CedarJsonConfig.h"
#include "CedarHelper.h"
#include "JsonHelper.h"
#include "CedarTimeHelper.h"
#include "CedarLogger.h"
#include "EnumStringMap.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "IncludeOnlyInMain.h"

namespace pt = boost::posix_time;

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

    //hacky: assume all exchanges open/close at the same time as SHSE
    Json::Value root;
    JsonHelper::loadJsonFile(
      "../../ShareConfig/MarketSpecification.json", root);

    JsonHelper::getStringArrayWithTag(root, "TradingSession.SHSE",
      "start", venueStarts);
    JsonHelper::getStringArrayWithTag(root, "TradingSession.SHSE",
      "end", venueEnds);
    mktSpecFormat= "%H:%M:%S";

    Json::Value ATickRoot;
    JsonHelper::loadJsonFile("../../ShareConfig/AShare.json", ATickRoot);

    for (unsigned i = 0; i < ATickRoot.size(); i++)
      ATickList.push_back(ATickRoot[i]["code"].asString());
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

    return 0;
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

  int testSmallOrder() {
    int numOrder = 10;
    std::string account = "3003_Stock";
    RequestType orderType = TYPE_SMALL_ORDER_REQUEST;
    std::string sendAddr = tradeServerMap["SmartOrderService"];
    std::vector<std::string> ids;

    OrderRequest req;
    req.set_response_address(respAddr);
    req.set_account(account);
    req.set_type(orderType);

    srand(time(NULL));
    int batch = 0;
    while (inMktSession()) {
      LOG(INFO) << "batch id: " << batch << std::endl;

      for (int i = 0; i < numOrder; i++) {
        ids.push_back(CedarHelper::getOrderId());
        req.set_id(ids[i]);
        req.set_alg_order_id(CedarHelper::getOrderId());
        req.set_batch_id(CedarHelper::getOrderId());

        int codeIndex = rand() % ATickList.size();
        std::string codeStr = ATickList[codeIndex];
        std::string name = codeStr.substr(0, 6);
        std::string venue = codeStr.substr(7, 2) + "SE";
        ExchangeType exchange = StringToEnum::toExchangeType(venue);

        req.set_code(name);
        req.set_exchange(exchange);
        req.set_buy_sell(LONG_BUY);
        int qty = 100 * ((rand() % 5) + 1);
        req.set_trade_quantity(qty);

        LOG(INFO) << "order " << i;
        LOG(INFO) << "name: " << codeStr;
        LOG(INFO) << "with qty: " << qty << std::endl;
        msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_ORDER_REQUEST, req));
      }
    batch++;
    sleep(300);
    }
    getchar();
    return 0;
  }

  bool inMktSession(){
    std::string now = CedarTimeHelper::getCurTimeStamp();
    pt::ptime ts = CedarTimeHelper::strToPTime("%H%M%S%F", now);
    now = CedarTimeHelper::ptimeToStr(mktSpecFormat, ts);

    for (unsigned i = 0; i < venueStarts.size(); i++) {
      if (now < venueStarts[i] && now > venueEnds[i]) {
        return true;
      }
    }

    return false;
  }

  int testFuturesToMultiAccount() {
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
    testSmallOrder();

    LOG(INFO) << "all sent" << std::endl;
    return 0;
  }

private:
  ProtoBufMsgHub msgHub;
  std::map<std::string, std::string> tradeServerMap;
  std::string respAddr;
  std::vector<std::string> venueStarts, venueEnds;
  std::string mktSpecFormat;
  std::vector<std::string> ATickList;
};

int main(int argc, char *argv[]) {
  CedarHelper::cedarAppInit(argc, argv);

  Tester tester;
  tester.run();
  return 0;
}
