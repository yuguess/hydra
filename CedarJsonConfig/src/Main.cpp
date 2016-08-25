#include "CedarJsonConfig.h"
#include "CedarHelper.h"
#include "IncludeOnlyInMain.h"
#include "CedarHelper.h"

int main(int argc, char *argv[]) {
  CedarHelper::cedarAppInit(argc, argv);

  LOG(INFO) << CedarJsonConfig::getInstance().hasMember("tt.tag1.deepArray");
  LOG(INFO) << CedarJsonConfig::getInstance().hasMember("tt.deepArray.hello");
  LOG(INFO) << CedarJsonConfig::getInstance().hasMember("tt.deepArray.ttt.tag");
  LOG(INFO) << CedarJsonConfig::getInstance().hasMember("tt");
  LOG(INFO) << CedarJsonConfig::getInstance().hasMember("Mode");
  LOG(INFO) << CedarJsonConfig::getInstance().hasMember("Mod2");

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
}
