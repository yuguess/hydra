#include <iostream>
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "CedarJsonConfig.h"
#include "CedarHelper.h"

int onMsg(MessageBase msg) {
  LOG(INFO) << "onMsg";
  if (msg.type() == TYPE_MARKETUPDATE)
    MarketUpdate mktUpdt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
  else
    LOG(WARNING) << "Recv invalid msg " << msg.type();

  return 0;
}

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  CedarHelper::initGlog("ProtoBufMsgHub");
  CedarJsonConfig::getInstance().loadConfigFile("config/ProtoBufMsgHub.json");
  ProtoBufMsgHub msgHub;

  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(onMsg);

  std::string pushAddr, publishAddr;

  CedarJsonConfig::getInstance().getStringByPath("DataSource.serverAddr",
      pushAddr);
  CedarJsonConfig::getInstance().getStringByPath("DataSource.boardcastAddr",
      publishAddr);

  DataRequest mdReq;
  mdReq.set_code("000001");
  mdReq.set_exchange("SZ");

  msgHub.pushMsg(pushAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST, mdReq));

  msgHub.addSubscription(publishAddr, mdReq.code());
  LOG(INFO) << "add subscription " << publishAddr << " " << mdReq.code();

  LOG(INFO) << "suspend";
  CedarHelper::blockSignalAndSuspend();
  msgHub.close();
  return 0;
}
