#include <iostream>
#include "CedarHelper.h"
#include "CedarLogging.h"
#include "ProtoBufMsgHub.h"
#include "DualRSI.h"
#include "BasicRSI.h"

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  CedarLogging::init("DualRSI");
  CedarJsonConfig::getInstance().loadConfigFile("./config/DualRSI.json");

  //DualRSI dualRSI;
  //dualRSI.run();

  BasicRSI basicRSI;
  basicRSI.run();

  LOG(INFO) << "init quiting procedures now!";
  google::protobuf::ShutdownProtobufLibrary();
}
