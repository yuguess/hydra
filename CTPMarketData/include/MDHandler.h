#ifndef MDHANDLER_H
#define MDHANDLER_H

#include <iostream>
#include <vector>
#include <map>

#include "ProtoBufMsgHub.h"

#include "ThostFtdcMdApi.h"
#include "ThostFtdcUserApiStruct.h"

class MDHandler : public CThostFtdcMdSpi {

public:
  MDHandler();
  ~MDHandler();
  int start();
  int close();

private:
  void OnFrontConnected();
  void OnFrontDisconnected(int nReason);
  void OnRspError(CThostFtdcRspInfoField *, int, bool);
  void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
  void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *,
      CThostFtdcRspInfoField *, int , bool);
  void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  void MarketOpenClose(int opencloseflag);
  int subscribeTicker(std::string);

  int onMsg(MessageBase msg);
  bool ctpMDtoCedarMD(CThostFtdcDepthMarketDataField*, MarketUpdate&);

  CThostFtdcMdApi *m_pUserMDApi;

  //const static int MAX_SYMBOL = 200;
  //std::vector<std::string> subscriber;
  //char* pSubscriber[MAX_SYMBOL];

  std::string mdFront;
  ProtoBufMsgHub msgHub;
  std::string m_TradingDay;
  std::string m_dataPath;

  std::map<std::string, std::string> codeToEx;
};

#endif
