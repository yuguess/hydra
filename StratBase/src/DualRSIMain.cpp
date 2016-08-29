#include "DualRSI.h"
#include "IncludeOnlyInMain.h"
#include "CedarHelper.h"

int main(int argc, char *argv[]) {
  CedarHelper::cedarAppInit(argc, argv);

  DualRSI dualRSI;
  dualRSI.run();

  LOG(INFO) << "init quiting procedures now!";
  google::protobuf::ShutdownProtobufLibrary();
}
