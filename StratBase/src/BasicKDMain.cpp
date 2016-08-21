#include "BasicKD.h"
#include "IncludeOnlyInMain.h"
#include "CedarHelper.h"

#include "PersistentState.h"
#include "CedarTimeHelper.h"

int main(int argc, char *argv[]) {
  CedarHelper::cedarAppInit(argc, argv);

  BasicKD basicKD;
  basicKD.run();

  LOG(INFO) << "complete";
  google::protobuf::ShutdownProtobufLibrary();
}
