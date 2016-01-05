#ifndef CEDAR_HELPER_H
#define CEDAR_HELPER_H

#include "glog/logging.h"
#include "json/json.h"
//#include "CedarConfig.h"
//#include "NewCedarConfig.h"
//#include "MsgHub.h"
#include <signal.h>
#include <fstream>
#ifdef __linux
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif
#ifdef _WIN32 
#include <winsock2.h>
#include <direct.h>
#endif

class CedarHelper {

public:
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

  //for xml configure.
  //static int getMsgHubConfig(std::vector<Channel> &chnls, std::string &bindPort) {
  //  std::vector<std::string> names;
  //  std::vector<std::string> hosts;

  //  ConfigGetValues("MsgHub.Subscribe", "Name", names);
  //  ConfigGetValues("MsgHub.Subscribe", "HostPort", hosts);
  //  bindPort = ConfigGetValue("MsgHub.Port");

  //  for (unsigned i = 0; i < names.size(); i++) {
  //    Channel chnl;
  //    chnl.name = names[i];
  //    chnl.hostPort = hosts[i];
  //    chnls.push_back(chnl);
  //  }

  //  return 0;
  //}

  static int stringSplit(std::string str, 
      char delimiter, std::vector<std::string> &res) {
    std::string token;
    std::istringstream ss(str);

    while (std::getline(ss, token, delimiter))
      res.push_back(token);

    return 0;
  }

  static std::string timestampString() {
    time_t sec;
    time(&sec);
    std::stringstream ss; 
    ss << sec;
    return ss.str();
  }

  //static int setupMsgHub(MsgHub &msgHub) {
  //  std::vector<Channel> chnls;
  //  std::string bindPort;
  //  getMsgHubConfig(chnls, bindPort);
  //  msgHub.init(chnls, bindPort);
  //  return 0;
  //}

  //for json configure.
  //static int getMsgHubConfig(std::vector<Channel> &chnls, 
  //    std::string &bindPort, Json::Value &jMsg) {
  //  bindPort = jMsg["Port"].asString();
  //  Json::Value Chan = jMsg["Channel"];
  //  std::vector<std::string> channelNames = Chan.getMemberNames();
  //  for (unsigned i = 0; i < channelNames.size(); i++) {
  //    Channel chnl;
  //    chnl.name = channelNames[i];
  //    chnl.hostPort = Chan[chnl.name].asString();
  //    chnls.push_back(chnl);
  //  }
  //  return 0;
  //}

  //static int setupMsgHub(MsgHub &msgHub,Json::Value &jMsg) {
  //  std::vector<Channel> chnls;
  //  std::string bindPort;

  //  getMsgHubConfig(chnls, bindPort,jMsg);
  //  msgHub.init(chnls, bindPort);
  //  return 0;
  //}

  ////for libconfig configure.
  //static int getMsgHubConfigNew(std::vector<Channel> &chnls, 
  //    std::string &bindPort) {
  //  std::string configName = "MsgHub";
  //  getMsgHubConfig(chnls, bindPort, configName);
  //  return 0;
  //}

  //static int getMsgHubConfig(std::vector<Channel> &chnls, 
  //    std::string &bindPort, std::string configName) {
  //  ConfigGetString(configName + ".Port", bindPort);
  //  std::cout << configName << " bind port " << bindPort << std::endl;
  //  libconfig::Setting& subs = ConfigGetSetting(configName + ".Subscribe");

  //  for (unsigned i = 0; i < subs.getLength(); i++) {
  //    libconfig::Setting& sub = subs[i];
  //    Channel chnl;
  //    if (sub.lookupValue("name", chnl.name) 
  //        && sub.lookupValue("hostPort", chnl.hostPort)) {
  //      std::cout << configName << " subscribe " << chnl.name 
  //        << " " << chnl.hostPort << std::endl;
  //      chnls.push_back(chnl);
  //    } else {
  //      std::cout << "msghub try to subscribe a invalid host and port " 
  //        << std::endl;
  //      exit(2);
  //    }
  //  }
  //  return 0;
  //}

  //static int setupMsgHubNew(MsgHub &msgHub) {
  //  std::vector<Channel> chnls;
  //  std::string bindPort;
  //  getMsgHubConfigNew(chnls, bindPort);
  //  msgHub.init(chnls, bindPort);
  //  return 0;
  //}

  //static int setupMsgHubWithConfigName(MsgHub &msgHub, std::string configName) {
  //  std::vector<Channel> chnls;
  //  std::string bindPort;
  //  getMsgHubConfig(chnls, bindPort, configName);
  //  msgHub.init(chnls, bindPort);
  //  return 0;
  //}

  //static NewOrderInfo returnInfoToNewOrderInfo(ReturnInfo &rtnInfo) {
  //  NewOrderInfo ordInfo;
  //  ordInfo.id = rtnInfo.id;
  //  ordInfo.side = rtnInfo.side;
  //  ordInfo.symbol = rtnInfo.symbol;
  //  ordInfo.price = rtnInfo.price;
  //  ordInfo.qty = rtnInfo.qty;

  //  return ordInfo;
  //}

  //static ReturnInfo newOrderInfoToReturnInfo(NewOrderInfo &ordInfo) {
  //  ReturnInfo rtnInfo;
  //  rtnInfo.id = ordInfo.id;
  //  rtnInfo.side = ordInfo.side;
  //  rtnInfo.symbol = ordInfo.symbol;
  //  rtnInfo.price = ordInfo.price;
  //  rtnInfo.qty = ordInfo.qty;

  //  return rtnInfo;
  //}

  static int initGlog(std::string program, int severity = google::GLOG_INFO) {
#ifdef _WIN32
    _mkdir("log");
#endif
#ifdef __linux
    mkdir("log",0777);
#endif
    //system("mkdir -p log");
    google::InitGoogleLogging(program.c_str());

    FLAGS_log_dir="./log";
    google::SetLogDestination(google::GLOG_INFO, 
        ("./log/" + program + "_INFO_").c_str());
    google::SetLogDestination(google::GLOG_WARNING, 
        ("./log/" + program + "_WARNING_").c_str());
    google::SetLogDestination(google::GLOG_ERROR, 
        ("./log/" + program + "_ERROR_").c_str());
    google::SetLogDestination(google::GLOG_FATAL, 
        ("./log/" + program + "_FATAL_").c_str());
    FLAGS_stderrthreshold = severity;

    FLAGS_colorlogtostderr=true;
    FLAGS_logbufsecs = 0;
    FLAGS_max_log_size = 100;
    FLAGS_stop_logging_if_full_disk = true;

    return 0;
  }

  //static std::string returnTypeToString(ReturnType returnType) {
  //  switch (returnType) {
  //    case ReturnType::CONFIRM: return "CONFIRM";
  //    case ReturnType::NEW_CONFIRM: return "NEW_CONFIRM";
  //    case ReturnType::CANCEL_CONFIRM: return "CANCEL_CONFIRM";
  //    case ReturnType::CEDAR_TRADE: return "CEDAR_TRADE";
  //    case ReturnType::CEDAR_ERROR: return "CEDAR_ERROR";
  //    default: return "Unknow";
  //  }
  //}

  static void getConfigRoot(std::string filepath, Json::Value& root) {
    std::ifstream config(filepath, std::ifstream::binary);
    Json::Reader reader;
    reader.parse(config, root, false);
  }

  //return current local time HHMMSSmmm
  static std::string getCurTimeStamp(){
    char currentTime[10];
#ifdef _WIN32 
    SYSTEMTIME st;
    GetLocalTime(&st);
    sprintf(currentTime, "%02d%02d%02d%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#endif
#ifdef __linux
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    int milli = curTime.tv_usec / 1000;
    char buffer [10];
    strftime(buffer, sizeof(buffer), "%H%M%S", localtime(&curTime.tv_sec));
    sprintf(currentTime, "%s%d", buffer, milli);
#endif
    return currentTime;
  }
};

#endif
