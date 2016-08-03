#include <iostream>
#include "CedarHelper.h"
#include "CedarLogging.h"
#include "ProtoBufMsgHub.h"
#include "Backtester.h"

int onMsg(MessageBase msg) {
  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mkt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    //LOG(INFO) << mkt.DebugString();
  } if (msg.type() == TYPE_ORDER_REQUEST) {
    OrderRequest req = ProtoBufHelper::unwrapMsg<OrderRequest>(msg);
    LOG(INFO) << req.DebugString();
  } if (msg.type() == TYPE_RESPONSE_MSG) {
    ResponseMessage rmsg = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
    LOG(INFO) << rmsg.DebugString();
  }

  return 0;
}

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  CedarLogging::init("Backtester");
  CedarJsonConfig::getInstance().loadConfigFile("./config/Backtester.json");

  Backtester bt;
  bt.registerCallback(std::bind(&onMsg, std::placeholders::_1));
  bt.run();

  LOG(INFO) << "init quiting procedures now!";
  google::protobuf::ShutdownProtobufLibrary();
}
