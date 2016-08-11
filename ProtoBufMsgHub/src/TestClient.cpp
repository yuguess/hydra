#include <iostream>
#include <vector>

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "CedarHelper.h"
#include "IncludeOnlyInMain.h"

inline double genRandom(int min, int max) {
  if (max < min)
    LOG(FATAL) << "Max " << max << " is less than min " << min;
  else if (max == min)
    return min;

  return static_cast<double>(rand() % (max - min));
}

class Tester {
public:
  Tester() : closeFlag(true) {
    ProtoBufHelper::setupProtoBufMsgHub(msgHub);
    msgHub.registerCallback(std::bind(&Tester::onMsg,
          this, std::placeholders::_1));

    subThr = std::thread(&Tester::run, this);
    subThr.detach();
  }

  ~Tester() {
    msgHub.close();
  }

  int onMsg(MessageBase msg) {
    LOG(INFO) << "onMsg";

    if (msg.type() == TYPE_MARKETUPDATE)
      MarketUpdate mktUpdt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    else if (msg.type() == TYPE_DATAREQUEST) {
      DataRequest dataReq = ProtoBufHelper::unwrapMsg<DataRequest>(msg);
      codes.push_back(dataReq.code());
      LOG(INFO) << "push " << dataReq.code() << " into codes";
    }

    return 0;
  }

  int run() {

    while (closeFlag) {
      sleep(1);
      MarketUpdate mktUpdt;

      if (codes.size() == 0)
        continue;

      std::string code = codes[genRandom(0, codes.size() - 1)];
      mktUpdt.set_code(code);
      for (int index = 0; index < 10; index++) {
        mktUpdt.add_bid_price(genRandom(1, 10));
        mktUpdt.add_bid_volume(genRandom(1, 10));
        mktUpdt.add_ask_price(genRandom(1, 10));
        mktUpdt.add_ask_volume(genRandom(1, 10));
      }
      std::string res =
        ProtoBufHelper::wrapMsg<MarketUpdate>(TYPE_MARKETUPDATE, mktUpdt);
      msgHub.boardcastMsg(code, res);
      LOG(INFO) << "boardcast " << res;
    }

  }

private:
  std::thread subThr;
  ProtoBufMsgHub msgHub;
  bool closeFlag;
  std::vector<std::string> codes;

};

int main(int argc, char *argv[]) {
  CedarHelper::cedarAppInit(argc, argv);

  Tester tester;

  LOG(INFO) << "suspend";
  CedarHelper::blockSignalAndSuspend();

  return 0;
}
