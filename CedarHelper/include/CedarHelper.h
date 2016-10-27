#ifndef CEDAR_HELPER_H
#define CEDAR_HELPER_H

#include "json/json.h"
#include <signal.h>
#include <fstream>
#include <ctype.h>
#ifdef __linux
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#endif
#ifdef _WIN32
#include <winsock2.h>
#include <direct.h>
#endif
#include "CedarJsonConfig.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "CmdLineParser.h"
#include "CedarLogger.h"
#include "JsonHelper.h"

class CedarHelper {

public:
  static int cedarAppInit(int argc, char *argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    std::string configPath;
    configPath = CmdLineParser::getConfigPathFromCmd(argc, argv);
    CedarJsonConfig::getInstance().loadConfigFile(configPath);
    CedarLogger::init();

    return 0;
  }

  static int stringSplit(std::string str,
      char delimiter, std::vector<std::string> &res) {
    std::string token;
    std::istringstream ss(str);

    while (std::getline(ss, token, delimiter))
      res.push_back(token);

    return 0;
  }

  static bool isStock(std::string code) {
    if (code.size() == 6 && isdigit(code[0]))
      return true;
    return false;
  }

  static int stockQtyRoundUp(int qty) {
    if (qty % 100 == 0)
      return qty;

    int integer = ((double)qty / stockMinimumQty);
    return integer * stockMinimumQty + stockMinimumQty;
  }

  static int stockQtyRoundDown(int qty) {
    if (qty % 100 == 0)
      return qty;

    int integer = ((double)qty / stockMinimumQty);
    return integer * stockMinimumQty;
  }

  static bool setupTradeServerMap(std::map<std::string, std::string> &accMap) {
    std::string configPath = "/home/infra/hydra/ShareConfig/ServerMap.json";

    Json::Value root;
    JsonHelper::loadJsonFile(configPath, root);

    std::vector<std::string> names, addrs;
    JsonHelper::getStringArrayWithTag(root, "TradeServer", "name", names);
    JsonHelper::getStringArrayWithTag(root, "TradeServer", "address", addrs);

    for (unsigned i = 0; i < names.size(); i++) {
      accMap[names[i]] = addrs[i];
    }

    return true;
  }

  static OrderRequest getInitOrderRequest(std::string respAddr,
    RequestType reqType) {

    OrderRequest req;
    std::string outOrderId = CedarHelper::getOrderId();

    req.set_response_address(respAddr);
    req.set_type(reqType);
    req.set_id(outOrderId);

    return req;
  }

#ifdef __linux
  //this only get IPV4 addr, skip 127.0.0.1
  static std::string getResponseAddr() {
    std::string pull;
    CedarJsonConfig::getInstance().getStringByPath("MsgHub.PullPort", pull);
    std::string ipStr;
    if (getHostIP(ipStr) != 0)
      LOG(ERROR) << "can't get this machine IP address";
    return ipStr + ":" + pull;
  }

  static int blockSignalAndSuspend() {
    int sig = 0, s = 0;
    sigset_t sigSet;
    sigemptyset(&sigSet);
    sigaddset(&sigSet, SIGINT);
    sigaddset(&sigSet, SIGTERM);
    sigaddset(&sigSet, SIGKILL);
    sigprocmask(SIG_BLOCK, &sigSet, NULL);

    if ((s = sigwait(&sigSet, &sig)) != 0)
      perror("sigwait error\n");
    return 0;
  }

  static std::string getOrderId() {
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    return std::to_string(curTime.tv_sec) + std::to_string(curTime.tv_usec);
  }
#endif

private:
  const static int stockMinimumQty = 100;

#ifdef __linux
  static int getHostIP(std::string &ip) {
    struct ifaddrs *ifAddrStruct = NULL;
    struct ifaddrs *ifa = NULL;
    void * tmpAddrPtr = NULL;

    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
      if (!ifa->ifa_addr) {
        continue;
      }

      if (ifa->ifa_addr->sa_family == AF_INET) {
        // check it is IP4 is a valid IP4 Address
        tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
        char addressBuffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

        if (strcmp(addressBuffer ,"127.0.0.1") == 0) {
          continue;
        }

        ip = std::string(addressBuffer);

        if (ifAddrStruct != NULL)
          freeifaddrs(ifAddrStruct);

        return 0;
      }
    }
    return -1;
  }
#endif

};

#endif
