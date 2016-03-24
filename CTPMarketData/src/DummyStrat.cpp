#include <iostream>
#include <vector>

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "CedarJsonConfig.h"
#include "ProtoBufMsgHub.h"
#include "CedarLogging.h"

class Tester {
public:
  Tester() {
    ProtoBufHelper::setupProtoBufMsgHub(msgHub);
    msgHub.registerCallback(std::bind(&Tester::onMsg, this, 
          std::placeholders::_1));

    std::vector<std::string> codes;
    std::vector<std::string> exchanges;
    CedarJsonConfig::getInstance().getStringArrayWithTag(codes, 
      "Ticker", "code");
    CedarJsonConfig::getInstance().getStringArrayWithTag(exchanges,
      "Ticker", "exchange");

    std::string dataServerAddr;
    std::string boardcastAddr;
    CedarJsonConfig::getInstance().getStringByPath("DataSource.serverAddr", 
      dataServerAddr);
    CedarJsonConfig::getInstance().getStringByPath("DataSource.boardcastAddr", 
      boardcastAddr);
    CedarJsonConfig::getInstance().getStringByPath("TradeSource.serverAddr", 
      tradeAddr);
    LOG(INFO) << "data server addr" << dataServerAddr;

    for (int i = 0; i < codes.size(); i++) {
      DataRequest mdReq;
      mdReq.set_code(codes[i]);
      mdReq.set_exchange(exchanges[i]);

      //send request
      msgHub.pushMsg(dataServerAddr, 
        ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST, mdReq));
      msgHub.addSubscription(boardcastAddr, mdReq.code());
    }
  }

  ~Tester() {
    msgHub.close();
  }

private:

  int onMsg(MessageBase msg) {
    LOG(INFO) << "onMsg";

    if (msg.type() == TYPE_MARKETUPDATE) {
      MarketUpdate mktUpdt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);

      //several dummy send at the same time expect to recv diff reply

      //10 tick and place bid and ask at far price, then cancel after 5 ticks
      BidCancel();
      
      //cross tick to hit and flat out after 100ticks
      
      //place 10 order at bid1 and wait 

      //first order
      
      //test market order

    } else if (msg.type() == TYPE_DATAREQUEST) {
      DataRequest dataReq = ProtoBufHelper::unwrapMsg<DataRequest>(msg);
      codes.push_back(dataReq.code());
      LOG(INFO) << "push " << dataReq.code() << " into codes";

    } else {
      LOG(WARNING) << "recv invalid mkt update with code";
    }

    return 0;
  }

  int BidCancel(MarketUpdate &mktUpdt) {
    static int count = 0;
    if (++count % 10 == 0) {
      OrderRequest req;

      if (!bidOrderFlag) {
        req.set_type(TYPE_LIMIT_ORDER_REQUEST);
        //req.set_limit_price();
        // 
        //send new order request
      } else {
        //send cancel order request
      }

      msgHub.pushMsg(tradeAddr,
          ProtoBufHelper::wrapMsg(TYPE_LIMIT_ORDER_REQUEST, req));
    }

    return 0;
  }

  std::string tradeAddr;
  std::thread subThr;
  ProtoBufMsgHub msgHub;
  std::vector<std::string> codes;

};

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  CedarLogging::init("DummyStrat");
  CedarJsonConfig::getInstance().loadConfigFile("./config/DummyStrat.json");

  Tester tester;

  LOG(INFO) << "suspend";
  CedarHelper::blockSignalAndSuspend();

  return 0;
}
