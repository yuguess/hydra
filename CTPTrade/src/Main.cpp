#include "TradeHandler.h"
#include "CedarHelper.h"
#include "IncludeOnlyInMain.h"

int main(int argc, char *argv[]) {
  CedarHelper::cedarAppInit(argc, argv);

  TradeHandler trade;
  trade.start();

  LOG(INFO) << "CTPTrade service online";
  CedarHelper::blockSignalAndSuspend();

  trade.close();

  LOG(INFO) << "init quiting procedures now!";
  google::protobuf::ShutdownProtobufLibrary();
}
