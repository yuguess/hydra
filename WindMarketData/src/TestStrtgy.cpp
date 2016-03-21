#include <iostream>
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "CedarJsonConfig.h"
#include "CedarHelper.h"
#include "CedarLogging.h"

int onMsg(MessageBase msg) {
  LOG(INFO) << "onMsg";

  if(msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mktUpdt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
  } else 
    LOG(WARNING) << "Recv invalid msg" << msg.type();

  return 0;
}

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION; 

  CedarLogging::init("TestStrtgy");
  CedarJsonConfig::getInstance().loadConfigFile("../config/TestStrtgy.json");

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























