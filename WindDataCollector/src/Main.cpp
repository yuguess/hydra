#include <iostream>
#include "ProtoBufMsgHub.h"
#include "CedarJsonConfig.h"
#include "CedarHelper.h"
#include "CedarLogging.h"
#include "WindDataCollector.h"

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  CedarLogging::init("WindDataCollector");
  CedarJsonConfig::getInstance().loadConfigFile("../config/WindDataCollector.json");
  
  WindDataCollector windDC; 
  windDC.start();

  LOG(INFO) << "suspend";
  CedarHelper::blockSignalAndSuspend();
  //msgHub.close();
  return 0;
}
