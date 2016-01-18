#include <iostream>
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "CedarJsonConfig.h"
#include "CedarHelper.h"
//#include "easylogging++.h"

int onMsg(MessageBase msg) {
  LOG(INFO) << "onMsg";

  if(msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mktUpdt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    std::cout << "szWindCode: " << mktUpdt.symbol() << std::endl;
    std::cout << "szCode: " << mktUpdt.code() << std::endl;
    std::cout << "exchange: " << mktUpdt.exchange() << std::endl;
    std::cout << "nOpen: " << mktUpdt.open_price() << std:: endl;
    std::cout << "nPreClose: " << mktUpdt.last_price() << std:: endl;
    std::cout << "nHigh: " << mktUpdt.highest_price() << std:: endl;
    std::cout << "nLow: " << mktUpdt.lowest_price() << std:: endl;
    std::cout << "nHighLimited: " << mktUpdt.high_limit_price() << std:: endl;
    std::cout << "nLowLimited: " << mktUpdt.low_limit_price() << std:: endl;
    std::cout << "nMatch: " << mktUpdt.open_interest() << std:: endl;
    std::cout << "iTurnover: " << mktUpdt.turnover() << std:: endl;
    std::cout << "nTime: " << mktUpdt.exchange_timestamp() << std:: endl;
    std::cout << "?recv_timestamp: " << mktUpdt.recv_timestamp() << std:: endl;

    for(int j = 0; j < 10; j++) {
      std::cout << "bid_price[" << j << "]: " << mktUpdt.bid_price(j) << std::endl;
      std::cout << "bid_volume[" << j << "]: " << mktUpdt.bid_volume(j) << std::endl;
      std::cout << "ask_price[" << j << "]: " << mktUpdt.ask_price(j) << std::endl;
      std::cout << "ask_volume[" << j << "]: " << mktUpdt.ask_volume(j) << std::endl;
    }
  } else 
    LOG(WARNING) << "Recv invalid msg" << msg.type();

  return 0;
}

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION; 

  //CedarHelper::initGlog("testStrtgy");
  CedarJsonConfig::getInstance().loadConfigFile("../config/testStrtgy.json");

  ProtoBufMsgHub msgHub;

  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(onMsg);

  std::string pushAddr = "127.0.0.1:15213";
  std::string pullAddr = "127.0.0.1:15212";

  DataRequest mdReq;
  mdReq.set_code("000001");
  mdReq.set_exchange("SH");

  msgHub.pushMsg(pushAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST, mdReq)); 
  msgHub.addSubscription(pullAddr, mdReq.code());
  LOG(INFO) << "add subscription" << pullAddr << " " << mdReq.code();  

  LOG(INFO) << "suspend";
  CedarHelper::blockSignalAndSuspend();
  msgHub.close();
  return 0;
}























