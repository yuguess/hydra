#include <iostream>
#include "TradeHandler.h"
#include "CedarHelper.h"
#include "CedarLogging.h"
#include "ProtoBufMsgHub.h"

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  CedarLogging::init("CTPTrade");
  CedarJsonConfig::getInstance().loadConfigFile("./config/CTPTrade.json");

  TradeHandler trade;
  trade.start();

  LOG(INFO) << "CTPTrade service online!";
  CedarHelper::blockSignalAndSuspend();

  trade.close();

  LOG(INFO) << "init quiting procedures now!";
  google::protobuf::ShutdownProtobufLibrary();
}
