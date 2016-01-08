#include <iostream>
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "CedarJsonConfig.h"
#include "CedarHelper.h"
#include "WindMarket.h"

//####ALL GLOBAL VARIABLE HERE####
//this is bad practice, but I have no better way
WindMarket winMarket;
//################################


int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  CedarHelper::initGlog("WindMarket");
  CedarJsonConfig::getInstance().loadConfigFile("config/WindMarketData.json");

  winMarket.start();

  //std::string pushAddr = "127.0.0.1:15216";
  //std::string pullAddr = "127.0.0.1:15215";
  //DataRequest mdReq;
  //mdReq.set_code("000001");
  //mdReq.set_exchange("SH");

  //msgHub.pushMsg(pushAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST, mdReq));

  //msgHub.addSubscription(pullAddr, mdReq.code());
  //LOG(INFO) << "add subscription " << pullAddr << " " << mdReq.code();

  LOG(INFO) << "suspend";
  CedarHelper::blockSignalAndSuspend();
  //msgHub.close();
  return 0;
}
