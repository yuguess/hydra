#include "IncludeOnlyInMain.h"
#include "CedarHelper.h"
#include "DualKD.h"

int main(int argc, char *argv[]) {
  CedarHelper::cedarAppInit(argc, argv);

  DualKD dualKD;
  dualKD.run();

  LOG(INFO) << "init quiting procedures now!";
  google::protobuf::ShutdownProtobufLibrary();
}
