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
  static void RecvData(THANDLE hTdf, TDF_MSG* pMsgHead);
  static void RecvSys(THANDLE hTdf, TDF_MSG* pSysMsg);
  static void RelayMarket(TDF_MARKET_DATA* pMarket, int nItems); 
  static void RelayTransaction(TDF_TRANSACTION* pTrans, int nItems); 

private:
  int addDataSubscription(DataRequest);
  int onMsg(MessageBase);

  TDF_OPEN_SETTING settings;
  TDF_ERR nErr;
  THANDLE nTDF;
  bool closeFlag;

  static ProtoBufMsgHub msgHub;

  const static int priceLevel = 10;
};

#endif
