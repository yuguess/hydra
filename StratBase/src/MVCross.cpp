#include <iostream>
#include <ta-lib/ta_libc.h>
#include "CedarHelper.h"
#include "CedarLogging.h"
#include "ProtoBufMsgHub.h"
#include "StratTemplate.h"
#include "assert.h"

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  //CedarLogging::init("MVCross");
  CedarJsonConfig::getInstance().loadConfigFile("./config/MVCross.json");

  StratTemplate mvCross;
  mvCross.run();

  LOG(INFO) << "init quiting procedures now!";
  google::protobuf::ShutdownProtobufLibrary();
}
