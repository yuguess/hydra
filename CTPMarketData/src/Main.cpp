#include <iostream>
#include "MDHandler.h"
//#include "CTPMarketData.h"
//#include "CedarConfig.h"
#include "CedarHelper.h"
#include <vector>
#include "CedarLogging.h"
#include "ProtoBufMsgHub.h"

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  CedarLogging::init("CTPMarketData");
  CedarJsonConfig::getInstance().loadConfigFile("./config/CTPMarketData.json");
  MDHandler md;

  md.start();

  LOG(INFO) << "CTPMarketData service online!";
  CedarHelper::blockSignalAndSuspend();

  md.close();

  LOG(INFO) << "init quiting procedures now!";
  google::protobuf::ShutdownProtobufLibrary();
}
