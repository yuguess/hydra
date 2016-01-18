#ifndef WIND_MARKET_H
#define WIND_MARKET_H

#include "TDFAPI.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"

class WindMarket {

public:
  WindMarket();
  ~WindMarket();
  int start();
  int close();
  

private:
  int addDataSubscription(DataRequest);
  int onMsg(MessageBase);

  TDF_OPEN_SETTING settings;
  TDF_ERR nErr;
  THANDLE nTDF;
  bool closeFlag;

  ProtoBufMsgHub msgHub;
};

#endif
