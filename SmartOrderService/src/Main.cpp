#include "SmartOrderService.h"
#include "CedarHelper.h"
#include "IncludeOnlyInMain.h"

int main(int argc, char *argv[]) {
  CedarHelper::cedarAppInit(argc, argv);

  SmartOrderService smartOrderService;
  LOG(INFO) << "Smart Order Service online!";
  CedarHelper::blockSignalAndSuspend();

  LOG(INFO) << "init quiting procedures now!";
  google::protobuf::ShutdownProtobufLibrary();
}
