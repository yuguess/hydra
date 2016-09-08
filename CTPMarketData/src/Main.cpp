#include <iostream>
#include "MDHandler.h"
#include "CedarHelper.h"
#include "IncludeOnlyInMain.h"

int main(int argc, char *argv[]) {
  CedarHelper::cedarAppInit(argc, argv);

  MDHandler md;
  md.start();

  LOG(INFO) << "Smart Order Service online!";
  CedarHelper::blockSignalAndSuspend();

  md.close();

  LOG(INFO) << "init quiting procedures now!";
  google::protobuf::ShutdownProtobufLibrary();
}
