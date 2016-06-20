#include <iostream>
#include <vector>

#include "ManualOrder.h"
#include "CedarJsonConfig.h"
#include "CedarHelper.h"
#include "CedarLogging.h"

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  CedarLogging::init("ManualOrder");
  CedarJsonConfig::getInstance().loadConfigFile("./config/ManualOrder.json");

  ManualOrder manualOrder;
  manualOrder.run();
  return 0;
}
