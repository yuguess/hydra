#ifndef WIND_MARKET_H
#define WIND_MARKET_H

class WindMarket {

public:
  WindMarket();
  int start();
  int close();

private:
  TDF_OPEN_SETTING settings;
  TDF_ERR nErr;
  THANDLE nTDF;

  bool closeFlag;

  ProtoBufMsgHub msgHub;
};

#endif
