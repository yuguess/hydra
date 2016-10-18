#include "MDHandler.h"
#include "CedarHelper.h"
#include "EnumStringMap.h"
#include "CedarTimeHelper.h"

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

  return 0;
}

int MDHandler::close() {
  //m_pUserMDApi->ReqUserLogout();
  return 0;
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
  LOG(WARNING) <<"Futures Market disconnected, OnFrontDisconnected, program quit!";
  exit(0);
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
}

int MDHandler::onMsg(MessageBase msg) {
  LOG(INFO) << "onMsg";
  if (msg.type() == TYPE_DATAREQUEST) {
    DataRequest dataReq = ProtoBufHelper::unwrapMsg<DataRequest>(msg);
    LOG(INFO) << "recv data request " << dataReq.code() << " into codes";
    subscribeTicker(dataReq.code());
    codeToEx[dataReq.code()] = EnumToString::toString(dataReq.exchange());
  } else {
    LOG(WARNING) << "recv invalid msg type " << msg.type();
  }

  return 0;
}

bool MDHandler::ctpMDtoCedarMD(CThostFtdcDepthMarketDataField *pMD,
    MarketUpdate &md) {

  md.set_trading_day(pMD->TradingDay);
  md.set_code(pMD->InstrumentID);
  //md.set_exchange(pMD->ExchangeID);
  md.set_recv_timestamp(CedarTimeHelper::getCurTimeStamp());

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

  return true;
}

int MDHandler::subscribeTicker(std::string ticker) {
  static std::map<std::string, int> subTickers;
  static char *tmpTicker[1];

  if (subTickers.find(ticker) != subTickers.end()) {
    LOG(INFO) << "ticker already subscribed" << ticker;
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
  //LOG(INFO) << std::string(pMD->ExchangeInstID) << "."
    //<< std::string(LOG(INFO)) << pMD->ExchangeID << " new update";

  MarketUpdate md;
  ctpMDtoCedarMD(pMD, md);
  std::string res = ProtoBufHelper::wrapMsg<MarketUpdate>(TYPE_MARKETUPDATE,md);
  std::string chan = md.code()+ "." + codeToEx[md.code()];
  msgHub.boardcastMsg(chan, res);

  LOG(INFO) << "onMarketUpdate";
}

void MDHandler::OnRspError(CThostFtdcRspInfoField *pRspInfo,
    int nRequestID, bool bIsLast) {
  if (pRspInfo->ErrorID != 0) {
    LOG(ERROR) << "Recv resp error " << " ErrorCode:"
                << pRspInfo->ErrorID << " ErrorMsg:" << pRspInfo->ErrorMsg;
  }
}
