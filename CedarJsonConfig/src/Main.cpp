#include "CedarJsonConfig.h"
#include "CedarHelper.h"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

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

  for (int i = 0; i < addrs.size(); i++) {
    std::cout << names[i] << std::endl;
    std::cout << addrs[i] << std::endl;
  }

  std::vector<std::string> fruits;
  CedarJsonConfig::getInstance().getStringArrayWithTag(
    fruits, "TestTag");

  for (int i = 0; i < fruits.size(); i++) {
    std::cout << fruits[i] << std::endl;
  }
  //LOG(ERROR) << "Error";
  //LOG(WARNING) <<  "WARNING";

  //create strategy object
  //strategy start 
  //getchar();
}
