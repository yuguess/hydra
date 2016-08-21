#include "CedarHelper.h"
#include "CedarStringHelper.h"
#include "LogHelper.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "IncludeOnlyInMain.h"
#include "CedarTimeHelper.h"
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
  LOG(INFO) << CedarTimeHelper::timestampString();
  LOG(INFO) << CedarTimeHelper::getCurTimeStamp();
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


  LOG(INFO) << CedarTimeHelper::timestampFormatString("%Y%m%d%H%M%S");

  testLogHelper();
  CedarHelper::blockSignalAndSuspend();
  LOG(INFO) << "ready to exit";

  //testCedarHelper();
  //testOrderId();
  //testIsStock();
}
