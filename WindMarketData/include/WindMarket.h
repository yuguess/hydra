#ifndef WIND_MARKET_H
#define WIND_MARKET_H

#include "TDFAPI.h"

class WindMarket {

public:
  WindMarket();
  ~WindMarket();
  int start();
  int close();

private:
  TDF_OPEN_SETTING_EXT settings;
  TDF_ERR nErr;
  THANDLE nTDF;

  bool closeFlag;

  ProtoBufMsgHub msgHub;
};

#endif
