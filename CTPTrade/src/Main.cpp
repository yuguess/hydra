#include <iostream>
#include <unistd.h>
#include "TradeHandler.h"
#include "CedarHelper.h"
#include "CedarLogging.h"
#include "ProtoBufMsgHub.h"

int printHelp() {
  std::cout << "\t-f (default with ./config/config file)\n"
      << "\t-n application name\n"
      << "\t-h print help message\n";
  return 0;
}

int main(int argc, char *argv[]) {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  std::string appName = "CTPTrade";
  std::string configPath =  "./config/CTPTrade.json";
  int opt = 0;
  while ((opt = getopt(argc, argv, ":f:n:h")) != -1) {
    switch(opt) {
      case 'f':
        configPath = optarg;
        break;
      case 'n':
        appName = optarg;
        break;
      case 'h':
        printHelp();
        return 0;
    }
  }
  CedarLogging::init(appName);
  CedarJsonConfig::getInstance().loadConfigFile(configPath);

  TradeHandler trade;
  trade.start();

  LOG(INFO) << "CTPTrade service online";
  CedarHelper::blockSignalAndSuspend();

  trade.close();

  LOG(INFO) << "init quiting procedures now!";
  google::protobuf::ShutdownProtobufLibrary();
}
