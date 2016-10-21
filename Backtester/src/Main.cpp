#include <iostream>
#include "CedarHelper.h"
#include "ProtoBufMsgHub.h"
#include "Backtester.h"
#include "IncludeOnlyInMain.h"

int onMsg(MessageBase msg) {
  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mkt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    LOG(INFO) << mkt.DebugString();
  } else if (msg.type() == TYPE_ORDER_REQUEST) {
    OrderRequest req = ProtoBufHelper::unwrapMsg<OrderRequest>(msg);
    LOG(INFO) << req.DebugString();
  } else if (msg.type() == TYPE_RESPONSE_MSG) {
    ResponseMessage rmsg = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
    LOG(INFO) << rmsg.DebugString();
  } else if (msg.type() == TYPE_RANGE_STAT) {
    RangeStat rangeStat = ProtoBufHelper::unwrapMsg<RangeStat>(msg);
    LOG(INFO) << rangeStat.DebugString();
    getchar();
  }

  return 0;
}

int main(int argc, char *argv[]) {
  CedarHelper::cedarAppInit(argc, argv);

  Backtester bt;
  bt.registerCallback(std::bind(&onMsg, std::placeholders::_1));

  Json::Value jsonConf;
  CedarJsonConfig::getInstance().getJsonValueByPath("Backtest", jsonConf);
  bt.run(jsonConf);

  LOG(INFO) << "init quiting procedures now!";
  google::protobuf::ShutdownProtobufLibrary();
}
