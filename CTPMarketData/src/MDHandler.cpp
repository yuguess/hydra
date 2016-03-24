#include "MDHandler.h"
//#include <string.h>
//#include <stdio.h>
//#include "NewCedarConfig.h"
//#include "CedarHelper.h"

MDHandler::MDHandler() {
  m_pUserMDApi = CThostFtdcMdApi::CreateFtdcMdApi();

  CedarJsonConfig::getInstance().getStringByPath("CTP.MDFront", mdFront);
  //ConfigGetString("dataPath", m_dataPath);

  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(std::bind(&MDHandler::onMsg, 
        this, std::placeholders::_1));
}

MDHandler::~MDHandler() {
  msgHub.close();
  //logout
  //release
}

int MDHandler::start() {
  //create user MDApi
  m_pUserMDApi->RegisterSpi(this);
  m_pUserMDApi->RegisterFront(const_cast<char*>(mdFront.c_str()));

  //FUTURES MARKET DATA CONNECTION POINT
  m_pUserMDApi->Init();
}

int MDHandler::close() {
  //m_pUserMDApi->ReqUserLogout();
}

// 1: MarketOpen, else: MarketClose
void MDHandler::MarketOpenClose(int opencloseflag) {
  //struct CThostFtdcDepthMarketDataField MD;
  //if(opencloseflag == 1) {
  //  memcpy(MD.InstrumentID, "BOD", sizeof(TThostFtdcInstrumentIDType));
  //} else {
  //  memcpy(MD.InstrumentID, "EOD", sizeof(TThostFtdcInstrumentIDType));
  //}

  //CDXSHAREDOBJ* pCEM = g_ShareMem.m_pSharedObj;

  //for(int acctID = 0;acctID<CDX_ACCOUNT_CNT;acctID++) {
  //  int AcctActive = (int)boost::interprocess::ipcdetail::atomic_read32(&pCEM->REGISTEREDACCT[acctID].active);
  //  if(AcctActive == 1) {
  //    pCEM->REGISTEREDACCT[acctID].MD.push(MD);
  //  }
  //}
}

void MDHandler::OnFrontConnected() {
  LOG(INFO) << "OnFrontConnected";
  CThostFtdcReqUserLoginField req;

  std::string brokerID, userID, psswd;
  CedarJsonConfig::getInstance().getStringByPath("CTP.BrokerID", brokerID);
  CedarJsonConfig::getInstance().getStringByPath("CTP.UserID", userID);
  CedarJsonConfig::getInstance().getStringByPath("CTP.Password", psswd);

  strcpy(req.BrokerID, brokerID.c_str());
  strcpy(req.UserID, userID.c_str());
  strcpy(req.Password, psswd.c_str());

  int result = m_pUserMDApi->ReqUserLogin(&req, 0);
  LOG(INFO) << "login result:" << result;
}

void MDHandler::OnFrontDisconnected(int nReason) {
  LOG(WARNING) <<"Futures Market disconnected, OnFrontDisconnected!";
}

void MDHandler::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

  LOG(INFO) << "Login ErrorCode:" << pRspInfo->ErrorID 
            << " ErrorMsg:" << pRspInfo->ErrorMsg
            << " RequestID:" << nRequestID
            << " Chain:" << bIsLast;

  if (pRspInfo->ErrorID != 0) {
    LOG(ERROR) << "Login failed ! check error code and msg";
    exit(0);
  } 

  LOG(INFO) << "MarketData API login successfully";
  //m_pUserMDApi->SubscribeMarketData(pSubscriber, subscriber.size());
  //m_TradingDay = pRspUserLogin->TradingDay;
  //for (int i = 0; i < subscriber.size(); i++) {
  //  std::string dataPath = m_dataPath + subscriber[i] + '/' + 
  //  m_TradingDay + '_' + subscriber[i]  ;
  //  symbolFile[subscriber[i]] = new std::ofstream(dataPath,
  //  std::ios::out|std::ios::binary|std::ios::app);
  //  if(!symbolFile[subscriber[i]] | !*symbolFile[subscriber[i]])
  //    LOG(FATAL)<<"cannot open file to write";
  //}
}

int MDHandler::onMsg(MessageBase msg) {
  if (msg.type() == TYPE_ORDER_REQUEST) {
    DataRequest dataReq = ProtoBufHelper::unwrapMsg<DataRequest>(msg);
    LOG(INFO) << "recv data request " << dataReq.code() << " into codes";
    subscribeTicker(dataReq.code());
  } else {
    LOG(WARNING) << "recv invalid msg type " << msg.type();
  }

  return 0;
}

bool MDHandler::ctpMDtoCedarMD(CThostFtdcDepthMarketDataField *pMD, 
    MarketUpdate &md) {

  md.set_trading_day(pMD->TradingDay);
  md.set_code(pMD->InstrumentID);
  md.set_exchange(pMD->ExchangeID);
  md.set_recv_timestamp(CedarHelper::getCurTimeStamp());

	md.set_last_price(pMD->LastPrice);
	md.set_pre_settlement_price(pMD->PreSettlementPrice);
	md.set_pre_close_price(pMD->PreClosePrice);
	md.set_pre_open_interest(pMD->PreOpenInterest);
	md.set_open_price(pMD->OpenPrice);
	md.set_highest_price(pMD->HighestPrice);
	md.set_lowest_price(pMD->LowestPrice);
	md.set_volume(pMD->Volume);
	md.set_turnover(pMD->Turnover);
	md.set_open_interest(pMD->OpenInterest);
	md.set_close_price(pMD->ClosePrice);
	md.set_settlement_price(pMD->SettlementPrice);
	md.set_high_limit_price(pMD->UpperLimitPrice);
	md.set_low_limit_price(pMD->LowerLimitPrice);
  md.set_average_price(pMD->AveragePrice);	

  //format like "09:30:00" --> "093000000"
  char exchangeTimestamp[10] = {0};
  char* updateTime = pMD->UpdateTime;
  sprintf(exchangeTimestamp,"%c%c%c%c%c%c%03d",updateTime[0],
  updateTime[1],updateTime[3],updateTime[4],updateTime[6],updateTime[7],
  pMD->UpdateMillisec);

  //right now, we only have 1level data
  md.add_bid_price(pMD->BidPrice1);
  md.add_bid_volume(pMD->BidVolume1);
  md.add_ask_price(pMD->AskPrice1);
  md.add_ask_volume(pMD->AskVolume1);
}

int MDHandler::subscribeTicker(std::string ticker) {
  static std::map<std::string, int> subTickers; 
  static char *tmpTicker[1];

  if (subTickers.find(ticker) != subTickers.end()) {
    LOG(WARNING) << "Subscribe a duplicate ticker " << ticker;
    return -1;
  }

  LOG(INFO) << "Send subscribe request " << ticker;

  tmpTicker[0] = const_cast<char*>(ticker.c_str());
  m_pUserMDApi->SubscribeMarketData(tmpTicker, 1);
  subTickers[ticker] = 0;

  return 0;
}

void MDHandler::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pInst,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

  if (pRspInfo->ErrorID == 0) {
    LOG(INFO) << "Successfully register " << pInst;
  } else {
    LOG(ERROR) << pInst << "Fail to register " << pInst << " ErrorCode:" 
                << pRspInfo->ErrorID;
  }
}

void MDHandler::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pMD) {
  MarketUpdate md;
  ctpMDtoCedarMD(pMD, md);
  std::string res = ProtoBufHelper::wrapMsg<MarketUpdate>(TYPE_MARKETUPDATE,md);
  msgHub.boardcastMsg(pMD->InstrumentID, res);

  LOG(INFO) << res;
}

void MDHandler::OnRspError(CThostFtdcRspInfoField *pRspInfo,
    int nRequestID, bool bIsLast) {
  if (pRspInfo->ErrorID != 0) {
    LOG(ERROR) << "Recv resp error " << " ErrorCode:" 
                << pRspInfo->ErrorID << " ErrorMsg:" << pRspInfo->ErrorMsg;
  }
}
