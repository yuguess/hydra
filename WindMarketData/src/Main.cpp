#include <iostream>
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "CedarJsonConfig.h"
#include "CedarHelper.h"
#include "WindMarket.h"
#include "CedarLogging.h"

//####ALL GLOBAL VARIABLE HERE####
//this is bad practice, but I have no better way
//WindMarket windMarket;
//################################


int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  CedarLogging::init();
  CedarJsonConfig::getInstance().loadConfigFile("../config/WindMarketData.json");
  //CedarHelper::initGlog("WindMarket");
  
  WindMarket windMarket; 
  windMarket.start();

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
