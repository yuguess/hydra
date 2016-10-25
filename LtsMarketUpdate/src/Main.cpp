#include "LtsMarketUpdate.h"
#include "CedarHelper.h"
#include "IncludeOnlyInMain.h"

int main(int argc, char *argv[]) {
  CedarHelper::cedarAppInit(argc, argv);

  LtsMarketUpdate ltsMd;
  LOG(INFO) << "LtsMarketUpdate Service online!";
  ltsMd.run();
  CedarHelper::blockSignalAndSuspend();

  LOG(INFO) << "init quiting procedures now!";
  google::protobuf::ShutdownProtobufLibrary();
}
