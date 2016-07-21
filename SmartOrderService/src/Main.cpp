#include "CedarHelper.h"
#include "CedarJsonConfig.h"
#include "CedarLogging.h"
#include "SmartOrderService.h"

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  CedarLogging::init("SmartOrderService");
  CedarJsonConfig::getInstance().loadConfigFile(
      "./config/SmartOrderService.json");

  SmartOrderService smartOrderService;
  LOG(INFO) << "Smart Order Service online!";
  CedarHelper::blockSignalAndSuspend();

  LOG(INFO) << "init quiting procedures now!";
  google::protobuf::ShutdownProtobufLibrary();
}
