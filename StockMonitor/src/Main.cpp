#include <map>

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "CedarHelper.h"
#include "CedarJsonConfig.h"
#include "ProtoBufMsgHub.h"
#include "StockProcessor.h"


int initMap(std::map<std::string, StockProcessor> &map) {
  std::vector<std::string> codes;
  CedarJsonConfig::getInstance().getStringArrayWithTag(codes, "Ticker", "code");

  for (int i = 0; i < codes.size(); i++) {
    StockProcessor stk = StockProcessor();
    stk.init(codes[i]);
    map[codes[i]] = stk;
  }

  return 0;
}

int onMsg(MessageBase msg) {
  static std::map<std::string, StockProcessor> codeToProcessor;
  static bool initFlag = true;
  if (initFlag) {
    initMap(codeToProcessor);
    initFlag = false;
    LOG(INFO) << "run ";
  }

  if (msg.type() == TYPE_DATAREQUEST) {
    DataRequest dataRequest = ProtoBufHelper::unwrapMsg<DataRequest>(msg);
  } else
    LOG(WARNING) << "Recv invalid msg with type " << msg.type();

  return 0;
}

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  CedarHelper::initGlog("StockMonitor");
  CedarJsonConfig::getInstance().loadConfigFile("config/StockMonitor.json");

  ProtoBufMsgHub msgHub;
  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(onMsg);

  //send data subscribeRequest
  std::string pushAddr;
  std::vector<std::string> codes;
  std::vector<std::string> exchanges;
  CedarJsonConfig::getInstance().getStringArrayWithTag(codes, "Ticker", "code");
  CedarJsonConfig::getInstance().getStringArrayWithTag(exchanges,
      "Ticker", "exchange");
  CedarJsonConfig::getInstance().getStringByPath("DataServer", pushAddr);

  LOG(INFO) << "data req push addr" << pushAddr;

  for (int i = 0; i < codes.size(); i++) {
    DataRequest mdReq;
    mdReq.set_code(codes[i]);
    mdReq.set_exchange(exchanges[i]);
    msgHub.pushMsg(pushAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST, mdReq));
  }

  CedarHelper::blockSignalAndSuspend();

  LOG(INFO) << "init quiting procedures now!"; 
  google::protobuf::ShutdownProtobufLibrary();
  msgHub.close();
}
