#include <iostream>
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "CedarJsonConfig.h"
#include "CedarHelper.h"
//#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

int onMsg(MessageBase msg) {
  LOG(INFO) << "onMsg";
  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mktUpdt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    double bdPrc = mktUpdt.bid_price(0);
    int bdVlm = mktUpdt.bid_volume(0);
    double ackPrc = mktUpdt.ask_price(0);
    int ackVlm = mktUpdt.ask_volume(0);
    int size = mktUpdt.bid_price_size();
    for (int index = 0; index < size; index++) {
        std::cout << "index: " << index << std::endl;
	std::cout << "bid_price: " << bdPrc << std::endl;
	std::cout << "bid_volume: " << bdVlm << std::endl;
	std::cout << "ask_price: " << ackPrc << std::endl;
	std::cout << "ask_volume: " << ackVlm << std::endl;
    }

  }
  else
    LOG(WARNING) << "Recv invalid msg " << msg.type();

  return 0;
}

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  //CedarHelper::initGlog("ProtoBufMsgHub");
  CedarJsonConfig::getInstance().loadConfigFile("../config/ProtoBufMsgHub.json");
  ProtoBufMsgHub msgHub;

  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(onMsg);

  std::string pushAddr = "127.0.0.1:15216";
  std::string pullAddr = "127.0.0.1:15215";
  DataRequest mdReq;
  mdReq.set_code("000001");
  mdReq.set_exchange("SH");

  msgHub.pushMsg(pushAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST, mdReq));

  msgHub.addSubscription(pullAddr, mdReq.code());
  LOG(INFO) << "add subscription " << pullAddr << " " << mdReq.code();

  LOG(INFO) << "suspend";
  CedarHelper::blockSignalAndSuspend();
  msgHub.close();
  return 0;
}
