#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "CedarHelper.h"
#include "CedarJsonConfig.h"
#include "ProtoBufMsgHub.h"
#include "StockProcessor.h"

int initMap(map<std::string, StockProcessor> &map) {
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
  static map<std::string, StockProcessor> codeToProcessor;
  static bool initFlag = true;
  if (initFlag) {
    initMap(codeToProcessor);
    initFlag = false;
    LOG(INFO) << "run ";
  }

  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mktUpdt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    if (codeToProcessor.find(mktUpdt.code()) == codeToProcessor.end()) {
      LOG(WARNING) << "Processor recv an invalid code " << mktUpdt.code();
      return -1;
    }
    codeToProcessor[mktUpdt.code()].onMarketUpdate(mktUpdt);

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
  std::vector<std::string> codes;
  std::vector<std::string> exchanges;
  CedarJsonConfig::getInstance().getStringArrayWithTag(codes, "Ticker", "code");
  CedarJsonConfig::getInstance().getStringArrayWithTag(exchanges,
      "Ticker", "exchange");
  std::string pushAddr = "127.0.0.1:15216";

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
