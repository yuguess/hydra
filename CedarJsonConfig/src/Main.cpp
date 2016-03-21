#include "CedarJsonConfig.h"
#include "CedarHelper.h"
#include "CedarLogging.h"

int main() {
//  CedarHelper::initGlog("CedarJsonConfig");
  CedarJsonConfig::getInstance().loadConfigFile("../config/CedarJsonConfig.json");

  std::string tmp;
  CedarJsonConfig::getInstance().getStringByPath("tt.deepArray.hello", tmp);
  LOG(INFO) << tmp;

  CedarJsonConfig::getInstance().getStringByPath("tt.deepArray.hello", tmp);
  LOG(INFO) << tmp;

  std::vector<std::string> names;
  std::vector<std::string> addrs;

  CedarJsonConfig::getInstance().getStringArrayWithTag(
    names, "MsgHub.Subscribe", "name");
  CedarJsonConfig::getInstance().getStringArrayWithTag(
    addrs, "MsgHub.Subscribe", "hostPort");

  std::vector<std::string> fruits;
  CedarJsonConfig::getInstance().getStringArrayWithTag(
    fruits, "TestTag");

  //LOG(ERROR) << "Error";
  //LOG(WARNING) <<  "WARNING";

  //create strategy object
  //strategy start 
  //getchar();
}
