//#include "CMdEngine.h"
//#include "CedarHelper.h"
//
//#include <iostream>
//#include <iomanip>
//#include <float.h>
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//
//using namespace std;
//
//CMdEngine::CMdEngine() {
//  m_pMdFrontMdApi = NULL;
//
//  strcpy(SHSE_CODE, "SSE");
//  strcpy(SZSE_CODE, "SZE");
//  isLogin = false;
//
//  std::vector<Channel> chnls;
//  std::string bindPort;
//  CedarHelper::getMsgHubConfig(chnls, bindPort);
//  msgHub.init(chnls, bindPort);
//
//  mdFront = ConfigGetValue("LTS.MDFront");
//}
//
//CMdEngine::~CMdEngine() {
//  m_pMdFrontMdApi->Release();
//}
//
//void CMdEngine::Init() {
//  m_pMdFrontMdApi = CSecurityFtdcMdApi::CreateFtdcMdApi();
//  m_pMdFrontMdApi->RegisterSpi(this);
//  m_pMdFrontMdApi->RegisterFront(const_cast<char*>(mdFront.c_str()));
//  m_pMdFrontMdApi->Init();
//}
//
//void CMdEngine::Join() {
//  m_pMdFrontMdApi->Join();
//}
//
//void CMdEngine::OnFrontConnected() {
//  CSecurityFtdcReqUserLoginField reqUserLogin;
//
//  strcpy(reqUserLogin.BrokerID, ConfigGetValue("LTS.BrokerID").c_str());
//  strcpy(reqUserLogin.UserID, ConfigGetValue("LTS.UserID").c_str());
//  strcpy(reqUserLogin.Password, ConfigGetValue("LTS.Password").c_str());
//
//  int iResult = m_pMdFrontMdApi->ReqUserLogin(&reqUserLogin, 1);
//  cerr << "--->>> login request : "
//    << iResult << ((iResult == 0) ? " success" : " fail") << endl;
//
//  cout << "subscribe MD" << endl;
//}
//
//void CMdEngine::OnRspUserLogin(CSecurityFtdcRspUserLoginField *pRspUserLogin,
//      CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
//
//  printf("Reached function %s.\n",__FUNCTION__);
//  printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
//  printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);
//  if (pRspInfo->ErrorID != 0) {
//    printf(">>>>>>> Chinese Equity MarketData API fail\n");
//    printf("Failed to login, errorcode=%d errormsg=%s requestid=%d chain=%d",
//    pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
//  } else {
//    printf(">>>>>>> Chinese Equity MarketData API success \n");
//    subscribeToMkt();
//  }
//}
//
//void CMdEngine::OnFrontDisconnected(int nReason) {
//  printf("Reached function: %s, China Equity Market Data Disconnection !!!\n",__FUNCTION__);
//}
//
//void CMdEngine::OnHeartBeatWarning(int nTimeLapse) {
//}
//
//void CMdEngine::OnRspError(CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
//  printf("Reached function: %s, Error Msg: %s\n",__FUNCTION__,pRspInfo->ErrorMsg);
//}
//
//void CMdEngine::OnRspUserLogout(CSecurityFtdcUserLogoutField *pUserLogout,
//      CSecurityFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
//  printf("Reached function: %s, Error Msg: %s\n",__FUNCTION__,pRspInfo->ErrorMsg);
//}
//
//
//void CMdEngine::ltsMDToCedarMD(CSecurityFtdcDepthMarketDataField *pMD, MarketUpdate &md) {
//  md.security = std::string(pMD->InstrumentID);
//  md.bidPrice.push_back(pMD->BidPrice1);
//  md.bidPrice.push_back(pMD->BidPrice2);
//  md.bidPrice.push_back(pMD->BidPrice3);
//  md.bidPrice.push_back(pMD->BidPrice4);
//  md.bidPrice.push_back(pMD->BidPrice5);
//  md.bidVolume.push_back(pMD->BidVolume1);
//  md.bidVolume.push_back(pMD->BidVolume2);
//  md.bidVolume.push_back(pMD->BidVolume3);
//  md.bidVolume.push_back(pMD->BidVolume4);
//  md.bidVolume.push_back(pMD->BidVolume5);
//  md.askPrice.push_back(pMD->AskPrice1);
//  md.askPrice.push_back(pMD->AskPrice2);
//  md.askPrice.push_back(pMD->AskPrice3);
//  md.askPrice.push_back(pMD->AskPrice4);
//  md.askPrice.push_back(pMD->AskPrice5);
//  md.askVolume.push_back(pMD->AskVolume1);
//  md.askVolume.push_back(pMD->AskVolume2);
//  md.askVolume.push_back(pMD->AskVolume3);
//  md.askVolume.push_back(pMD->AskVolume4);
//  md.askVolume.push_back(pMD->AskVolume5);
//}
//
//void CMdEngine::OnRtnDepthMarketData(CSecurityFtdcDepthMarketDataField *pMD){
//
//  MarketUpdate md;
//  ltsMDToCedarMD(pMD, md);
//  std::string channel(pMD->InstrumentID);
//  std::string msg(md.serialize());
//  msgHub.send(channel, msg);
//
//}
//
//void CMdEngine::subscribeToMkt() {
//
//  if (SSEStocksubscriber.size() > 0) {
//    for (int i = 0; i < SSEStocksubscriber.size(); i++) {
//      printf("SH stock %s\n", pSSEStockSubscriber[i]);
//    }
//
//    m_pMdFrontMdApi->SubscribeMarketData(pSSEStockSubscriber, SSEStocksubscriber.size(), SHSE_CODE);
//  }
//
//  if (SZEStocksubscriber.size() > 0) {
//    for (int i = 0; i < SZEStocksubscriber.size(); i++) {
//      printf("SZ stock %s\n", pSZEStockSubscriber[i]);
//    }
//
//    m_pMdFrontMdApi->SubscribeMarketData(pSZEStockSubscriber, SZEStocksubscriber.size(), SZSE_CODE);
//  }
//
//  if (SSEIndexsubscriber.size() > 0) {
//    for (int i = 0; i < SSEIndexsubscriber.size(); i++) {
//      printf("SH stock %s\n", pSSEIndexSubscriber[i]);
//    }
//
//    m_pMdFrontMdApi->SubscribeMarketData(pSSEIndexSubscriber, SSEIndexsubscriber.size(), SHSE_CODE);
//  }
//
//  if (SZEIndexsubscriber.size() > 0) {
//    for (int i = 0; i < SZEIndexsubscriber.size(); i++) {
//      printf("SZ stock %s\n", pSZEIndexSubscriber[i]);
//    }
//
//    m_pMdFrontMdApi->SubscribeMarketData(pSZEIndexSubscriber, SZEIndexsubscriber.size(), SZSE_CODE);
//  }
//
//}
//
//int CMdEngine::populateSubscribers(std::vector<std::string> &SSEStk,
//    std::vector<std::string> &SZEStk, std::vector<std::string> &SSEIdx, std::vector<std::string> &SZEIdx) {
//
//  SSEStocksubscriber = SSEStk;
//  SZEStocksubscriber = SZEStk;
//  SSEIndexsubscriber = SSEIdx;
//  SZEIndexsubscriber = SZEIdx;
//
//  for (int i = 0; i < SSEStk.size(); i++) {
//    pSSEStockSubscriber[i] = const_cast<char*>(SSEStk[i].c_str());
//    printf("SSE stck %s\n", pSSEStockSubscriber[i]);
//  }
//
//  for (int i = 0; i < SZEStk.size(); i++) {
//    pSZEStockSubscriber[i] = const_cast<char*>(SZEStk[i].c_str());
//    printf("SZE stck %s\n", pSZEStockSubscriber[i]);
//  }
//
//  for (int i = 0; i < SSEIdx.size(); i++)
//    pSSEIndexSubscriber[i] = const_cast<char*>(SSEIdx[i].c_str());
//
//  for (int i = 0; i < SZEIdx.size(); i++)
//    pSZEIndexSubscriber[i] = const_cast<char*>(SZEIdx[i].c_str());
//}
//
//bool CMdEngine::ValidValue(double val) {
//  if(val > EquityBigPositiveNumber || val < EquityBigNegativeNumber) {
//    return false;
//  } else {
//    return true;
//  }
//}
//
//bool CMdEngine::MarketDataTimeout(char* pTimeStamp, char* pExch) {
//  if(strcmp(pExch,SHSE_CODE) == 0) {
//    if((strcmp(pTimeStamp,"15:00:00") > 0 || strcmp(pTimeStamp,"09:30:00") < 0))
//      return false;
//    else
//      return true;
//  } else if(strcmp(pExch,SZSE_CODE) == 0) {
//    if((strcmp(pTimeStamp,"14:57:00") > 0 || strcmp(pTimeStamp,"09:30:00") < 0))
//      return false;
//    else
//      return true;
//  } else {
//    return false;
//  }
//}
