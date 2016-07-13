#include <map>

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "CedarHelper.h"
#include "CedarJsonConfig.h"
#include "ProtoBufMsgHub.h"
#include "StockProcessor.h"
#include "CedarLogging.h"

int initMap(std::map<std::string, StockProcessor> &map) {
  std::vector<std::string> codes, exchanges;
  CedarJsonConfig::getInstance().getStringArrayWithTag(codes, "Ticker",
      "code");
  CedarJsonConfig::getInstance().getStringArrayWithTag(exchanges, "Ticker",
      "exchange");

  for (int i = 0; i < codes.size(); i++) {
    StockProcessor stk = StockProcessor();
    std::string chan = codes[i] + "." + exchanges[i];
    stk.init(chan);
    map[chan] = stk;
  }

  return 0;
}

int onMsg(MessageBase msg) {
  static std::map<std::string, StockProcessor> codeToProcessor;
  static bool initFlag = true;
  if (initFlag) {
    initMap(codeToProcessor);
    initFlag = false;
    LOG(INFO) << "run";
  }

  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mkt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    std::string chan = mkt.code() + "." +
      CedarHelper::exchangeTypeToString(mkt.exchange());
    LOG(INFO) << mkt.DebugString();



    //if (codeToProcessor.find(chan) == codeToProcessor.end()) {
    //  LOG(WARNING) << "recv invalid mkt update with code" << chan;
    //} else {
    //  //LOG(INFO) << "recv " << chan << " update";
    //  codeToProcessor[chan].onMarketUpdate(mkt);
    //}

  } else
    LOG(WARNING) << "Recv invalid msg with type " << msg.type();

  return 0;
}

int main() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  CedarLogging::init("StockMonitor");
  CedarJsonConfig::getInstance().loadConfigFile("./config/StockMonitor.json");

  ProtoBufMsgHub msgHub;
  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(onMsg);

  std::vector<std::string> codes;
  std::vector<std::string> exchanges;
  CedarJsonConfig::getInstance().getStringArrayWithTag(codes, "Ticker", "code");
  CedarJsonConfig::getInstance().getStringArrayWithTag(exchanges,
      "Ticker", "exchange");

  std::string dataServerAddr;
  std::string boardcastAddr;
  CedarJsonConfig::getInstance().getStringByPath("DataServer.serverAddr",
      dataServerAddr);
  CedarJsonConfig::getInstance().getStringByPath("DataServer.boardcastAddr",
      boardcastAddr);
  LOG(INFO) << "data server addr" << dataServerAddr;

  for (int i = 0; i < codes.size(); i++) {
    DataRequest mdReq;
    mdReq.set_code(codes[i]);
    mdReq.set_exchange(CedarHelper::stringToExchangeType(exchanges[i]));

    LOG(INFO) << "push msg to dataServer addr" << dataServerAddr;

    msgHub.pushMsg(dataServerAddr,
        ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST, mdReq));

    std::string chan = mdReq.code() + "." + exchanges[i];
    msgHub.addSubscription(boardcastAddr, mdReq.code());
  }

  LOG(INFO) << "StockMonitor service online!";
  CedarHelper::blockSignalAndSuspend();

  LOG(INFO) << "init quiting procedures now!";
  google::protobuf::ShutdownProtobufLibrary();
  msgHub.close();
}
