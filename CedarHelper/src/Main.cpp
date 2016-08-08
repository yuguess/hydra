#include "CedarHelper.h"
#include "CedarLogging.h"
#include "CedarStringHelper.h"
#include "LogHelper.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include <chrono>

bool testLogHelper() {
  ResponseMessage rsp;
  rsp.set_id("1");
  rsp.set_ref_id("2");
  rsp.set_type(TYPE_NEW_ORDER_CONFIRM);
  rsp.set_trade_quantity(1);
  rsp.set_error_code(0);
  rsp.set_error_msg("test error msg");

  LogHelper::logObject(rsp);

  return true;
}

bool testCedarHelper() {
  LOG(INFO) << CedarHelper::timestampString();
  LOG(INFO) << CedarHelper::getCurTimeStamp();
  return true;
}

bool testOrderId() {
  LOG(INFO) << CedarHelper::getOrderId();
  return true;
}

bool testIsStock() {
  LOG(INFO) << CedarHelper::isStock("601398");
  LOG(INFO) << CedarHelper::isStock("MA1398");
  LOG(INFO) << CedarHelper::isStock("000000");
  return true;
}

int main(int argc, char *argv[]) {
  CedarHelper::cedarAppInit(argc, argv);

  LOG(DEBUG) << "DEBUG";
  LOG(INFO) << "INFO";
  LOG(WARNING) << "WARNING";
  LOG(ERROR) << "ERROR";

  //std::vector<std::string> res;
  //std::string testStr = "    asdf,asdf,asdf123sdf, asdf,123,   123, sss";
  //split(testStr, ',', res);
  //for (unsigned i = 0; i < res.size(); i++)
  //LOG(INFO) << res[i];

  //testLogHelper();
  //testCedarHelper();
  //testOrderId();
  //testIsStock();
}
