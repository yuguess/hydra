#include "TDFAPI.h"
#include "ProtoBufMsgHub.h"
#include "WindMarket.h"

extern WindMarket windMarket;

void RelayMarket(TDF_MARKET_DATA* pMarket, int nItems);
void RelayFuture(THANDLE hTdf,TDF_FUTURE_DATA* pFuture, int nItems);
void RelayIndex(TDF_INDEX_DATA* pIndex, int nItems);
void RelayTransaction(TDF_TRANSACTION* pTransaction, int nItems);
void RelayOrder(TDF_ORDER* pOrder, int nItems);
void RelayOrderQueue(TDF_ORDER_QUEUE* pOrderQueue, int nItems);

#define ELEM_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))
#define SAFE_STR(str) ((str)?(str):"")
#define SAFE_CHAR(ch) ((ch) ? (ch) : ' ')

char* chararr2str(char* szBuf, int nBufLen, char arr[], int n) {
  int nOffset = 0;
  for (int i=0; i<n; i++) {
    nOffset += snprintf(szBuf+nOffset, nBufLen-nOffset, "%d(%c) ", 
        arr[i], SAFE_CHAR(arr[i]));
  }
  return szBuf;
}

char* intarr2str(char* szBuf, int nBufLen, int arr[], int n) {
  int nOffset = 0;
  for (int i=0; i<n; i++) {
    nOffset += snprintf(szBuf+nOffset, nBufLen-nOffset, "%d ", arr[i]);
  }
  return szBuf;
}

int WindMarket::onMsg(MessageBase msg) {
  LOG(INFO) << "onMsg";
  if (msg.type() == TYPE_DATAREQUEST) {
    DataRequest dtRqst = ProtoBufHelper::unwrapMsg<DataRequest>(msg);
    addDataSubscription(dtRqst);
  } else
    LOG(WARNING) << "Recv invalid msg " << msg.type();

  return 0;
}

#define GETRECORD(pBase, TYPE, nIndex) ((TYPE*)((char*)(pBase) + sizeof(TYPE)*(nIndex)))
#define PRINTNUM  1
static int recordNum = 0;
void WindMarket::RecvData(THANDLE hTdf, TDF_MSG* pMsgHead) {
  if (!pMsgHead->pData) {
    assert(0);
    return ;
  }

  unsigned int nItemCount = pMsgHead->pAppHead->nItemCount;
  unsigned int nItemSize = pMsgHead->pAppHead->nItemSize;
  if (!nItemCount) {
    //assert(0);
    return ;
  }

  recordNum++;
  switch (pMsgHead->nDataType) {

    case MSG_DATA_MARKET: {
      assert(nItemSize == sizeof(TDF_MARKET_DATA));
      if (recordNum > PRINTNUM){
        recordNum = 0;
    	windMarket.RelayMarket((TDF_MARKET_DATA*)pMsgHead->pData, nItemCount);
        //DumpScreenMarket((TDF_MARKET_DATA*)pMsgHead->pData, nItemCount);
        std::cout << "MarketDataUpdate broadcast" << std::endl;
      }
    }
    break;

    case MSG_DATA_FUTURE: {
      assert(nItemSize == sizeof(TDF_FUTURE_DATA));
      if (recordNum > PRINTNUM) {
        recordNum = 0;
        //DumpScreenFuture(hTdf,(TDF_FUTURE_DATA*)pMsgHead->pData, nItemCount);
        std::cout << "Future" << std::endl;
      }
      TDF_FUTURE_DATA* pLastFuture = GETRECORD(pMsgHead->pData, 
          TDF_FUTURE_DATA, nItemCount-1);
      printf( "接收到期货行情记录:代码：%s, 业务发生日:%d, 时间:%d, 最新价:%d, \
         持仓总量:%lld \n", pLastFuture->szWindCode, pLastFuture->nActionDay, 
         pLastFuture->nTime, pLastFuture->nMatch, pLastFuture->iOpenInterest);
    }
    break;

    case MSG_DATA_INDEX: {
      if (recordNum > PRINTNUM){
        recordNum = 0;
        //DumpScreenIndex((TDF_INDEX_DATA*)pMsgHead->pData, nItemCount);
        std::cout << "Index" << std::endl;
      }

      TDF_INDEX_DATA* pLastIndex = GETRECORD(pMsgHead->pData,TDF_INDEX_DATA,
          nItemCount - 1);
      printf( "接收到指数记录:代码：%s, 业务发生日:%d, 时间:%d, 最新指数:%d, \
          成交总量:%lld \n", pLastIndex->szWindCode, pLastIndex->nActionDay, 
          pLastIndex->nTime, pLastIndex->nLastIndex, pLastIndex->iTotalVolume);
    }
    break;

    case MSG_DATA_TRANSACTION: {
      if (recordNum > PRINTNUM) {
        recordNum = 0;
        //DumpScreenTransaction((TDF_TRANSACTION*)pMsgHead->pData, nItemCount);
    	windMarket.RelayTransaction((TDF_TRANSACTION*)pMsgHead->pData, nItemCount);
	std::cout << "ScreenTransaction" << std::endl;
      }
      TDF_TRANSACTION* pLastTransaction = GETRECORD(pMsgHead->pData,
          TDF_TRANSACTION, nItemCount-1);
      printf( "接收到逐笔成交记录:代码：%s, 业务发生日:%d, 时间:%d, \
          成交价格:%d, 成交数量:%d \n", pLastTransaction->szWindCode,
        pLastTransaction->nActionDay, pLastTransaction->nTime, 
        pLastTransaction->nPrice, pLastTransaction->nVolume);
    }
    break;

    case MSG_DATA_ORDERQUEUE: {
      if (recordNum > PRINTNUM){
        recordNum = 0;
        //DumpScreenOrderQueue((TDF_ORDER_QUEUE*)pMsgHead->pData, nItemCount);
      }

      TDF_ORDER_QUEUE* pLastOrderQueue = GETRECORD(pMsgHead->pData,
          TDF_ORDER_QUEUE, nItemCount-1);
      printf( "接收到委托队列记录:代码：%s, 业务发生日:%d, 时间:%d, \
        委托价格:%d, 订单数量:%d \n", pLastOrderQueue->szWindCode, 
        pLastOrderQueue->nActionDay, pLastOrderQueue->nTime, 
        pLastOrderQueue->nPrice, pLastOrderQueue->nOrders);
    }
    break;

    case MSG_DATA_ORDER: {
      if (recordNum > PRINTNUM){
        recordNum = 0;
        //DumpScreenOrder((TDF_ORDER*)pMsgHead->pData, nItemCount);
      }

#if 0
      for (int i=0; i<pMsgHead->pAppHead->nItemCount; i++) {
        TDF_ORDER* pOrder = GETRECORD(pMsgHead->pData, TDF_ORDER, i);

        if (strcmp(pOrder->szWindCode, "000001.SZ")==0) {
          if (pOrder->nPrice<0) {
            int j = 1;
          }
          static int last_time = 0;
          static int index = 0;

          if (last_time/1000==pOrder->nTime/1000)
            index++;
          else 
            index = 1;

          last_time = pOrder->nTime;
          index++;

          printf("逐笔委托: %s %d %d %d %d %d \n", pOrder->szWindCode,
            pOrder->nActionDay, pOrder->nTime, index, pOrder->nPrice, 
            pOrder->nVolume);
        }
      }
#endif

      TDF_ORDER* pLastOrder = 
        GETRECORD(pMsgHead->pData, TDF_ORDER, nItemCount-1);

      printf("接收到逐托记代码：%s, 业务发生日:%d, 时间:%d, 委托价格:%d,\
        委托数量:%d \n", pLastOrder->szWindCode, pLastOrder->nActionDay, 
      pLastOrder->nTime, pLastOrder->nPrice, pLastOrder->nVolume);
    }
    break;

    default:
    assert(0);
    break;
  }
}

void WindMarket::RecvSys(THANDLE hTdf, TDF_MSG* pSysMsg) {
  if (!pSysMsg ||! hTdf) {
    return;
  }

  switch (pSysMsg->nDataType) {
  case MSG_SYS_DISCONNECT_NETWORK:
    break;

  case MSG_SYS_CONNECT_RESULT: {
  }
  break;

  case MSG_SYS_LOGIN_RESULT: {

  }
  break;

  case MSG_SYS_CODETABLE_RESULT: {
  }
  break;

  case MSG_SYS_QUOTATIONDATE_CHANGE: {
  }
  break;

  case MSG_SYS_MARKET_CLOSE: {
  }
  break;

  case MSG_SYS_HEART_BEAT:
  break;

  default:
    assert(0);
    break;
  }
}


void WindMarket::RelayMarket(TDF_MARKET_DATA* pMarket, int nItems) {

  MarketUpdate mktUpdt;
  for (int i = 0; i < nItems; i++) {
    const TDF_MARKET_DATA& marketData = pMarket[i];
    const char *exchange = marketData.szWindCode + 7;

    mktUpdt.set_symbol(marketData.szWindCode);
    mktUpdt.set_code(marketData.szCode); 
    mktUpdt.set_exchange(exchange);
    mktUpdt.set_status(marketData.nStatus);
    mktUpdt.set_pre_close(marketData.nPreClose);
    mktUpdt.set_open_price(marketData.nOpen);
    mktUpdt.set_highest_price(marketData.nHigh);
    mktUpdt.set_lowest_price(marketData.nLow);
    mktUpdt.set_high_limit_price(marketData.nHighLimited);
    mktUpdt.set_low_limit_price(marketData.nLowLimited);
    mktUpdt.set_open_interest(1024); //???
    mktUpdt.set_latest_price(marketData.nMatch);
    mktUpdt.set_num_trades(marketData.nNumTrades);
    mktUpdt.set_turnover(marketData.iTurnover);
    mktUpdt.set_volume(marketData.iVolume);
    mktUpdt.set_total_bid_vol(marketData.nTotalBidVol);
    mktUpdt.set_total_ask_vol(marketData.nTotalAskVol);
    mktUpdt.set_weighted_avg_bid_price(marketData.nWeightedAvgBidPrice);
    mktUpdt.set_weighted_avg_ask_price(marketData.nWeightedAvgAskPrice);
    mktUpdt.set_iopv(marketData.nIOPV);
    mktUpdt.set_yield_to_maturity(marketData.nYieldToMaturity);
    mktUpdt.set_action_day(marketData.nActionDay);
    mktUpdt.set_exchange_timestamp(std::to_string(marketData.nTime));
    mktUpdt.set_recv_timestamp("1024"); //???

    for (int j = 0; j < 10; j++) {
      mktUpdt.add_bid_price(marketData.nBidPrice[j]);
      mktUpdt.add_bid_volume(marketData.nBidVol[j]);
      mktUpdt.add_ask_price(marketData.nAskPrice[j]);
      mktUpdt.add_ask_volume(marketData.nAskVol[j]);
    }
    std::string res = 
      ProtoBufHelper::wrapMsg<MarketUpdate>(TYPE_MARKETUPDATE, mktUpdt); 
    msgHub.boardcastMsg(marketData.szCode, res);
  }   

}

void WindMarket::RelayTransaction(TDF_TRANSACTION* pMarket, int nItems) {
  Transaction trans;
  for (int i = 0; i < nItems; i++) {
    const TDF_TRANSACTION& tdfTrans = pMarket[i];
    const char *exchange = tdfTrans.szWindCode + 7;

    trans.set_code(tdfTrans.szCode);
    trans.set_symbol(tdfTrans.szWindCode);
    trans.set_exchange(exchange);
    trans.set_action_day(tdfTrans.nActionDay);
    trans.set_time(tdfTrans.nTime);
    trans.set_index(tdfTrans.nIndex);
    trans.set_price(tdfTrans.nPrice);
    trans.set_volume(tdfTrans.nVolume);
    trans.set_turnover(tdfTrans.nTurnover);
    trans.set_bsflag(tdfTrans.nBSFlag);
    trans.set_ch_order_kind(&(tdfTrans.chOrderKind));
    trans.set_ch_function_code(&(tdfTrans.chFunctionCode));

    trans.set_ask_order(tdfTrans.nAskOrder);
    trans.set_bid_order(tdfTrans.nBidOrder);

    std::string res = 
      ProtoBufHelper::wrapMsg<Transaction>(TYPE_TRANSACTION, trans);
    msgHub.boardcastMsg(tdfTrans.szCode, res);
  }
}

int WindMarket::addDataSubscription(DataRequest dtRqst) {
  std::string windTicker = dtRqst.code() + "." + dtRqst.exchange();
  if (TDF_SetSubscription(nTDF, windTicker.c_str(), SUBSCRIPTION_ADD) != 0) {
    LOG(WARNING) << "subscribe " << windTicker << " failed";
    return -1;
  }

  return 0;
}

WindMarket::WindMarket() {
  closeFlag = false;
}

WindMarket::~WindMarket() {
  if (!closeFlag)
    close();
}

int WindMarket::start() {
  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(std::bind(&WindMarket::onMsg,
          this, std::placeholders::_1));


  std::string srvrIp, srvrPrt, usr, psswrd; 

  CedarJsonConfig::getInstance().getStringByPath("WindAccount.ServerIP", 
      srvrIp);
  CedarJsonConfig::getInstance().getStringByPath("WindAccount.ServerPort", 
      srvrPrt);
  CedarJsonConfig::getInstance().getStringByPath("WindAccount.User",
      usr);
  CedarJsonConfig::getInstance().getStringByPath("WindAccount.Password",
      psswrd);

  strcpy(settings.szIp, srvrIp.c_str());
  strcpy(settings.szPort, srvrPrt.c_str());
  strcpy(settings.szUser, usr.c_str());
  strcpy(settings.szPwd,  psswrd.c_str());

  //settings.nReconnectCount = 99999999;
  //settings.nReconnectGap = 5;

  //set data callback func
  settings.pfnMsgHandler = RecvData; 
  //set system msg callback func
  settings.pfnSysMsgNotify = RecvSys;

  //settings.nProtocol = 0;
  //需要订阅的市场列表
  settings.szMarkets = "SZ-2;";
  //settings.szMarkets = "SZ;SH";

  //需要订阅的股票,为空则订阅全市场
  settings.szSubScriptions = "000001.SZ;000002.SH";

  //请求的日期，格式YYMMDD，为0则请求今天
  //请求的时间，格式HHMMSS，为0则请求实时行情，为0xffffffff从头请求
  //settings.nDate = 0;
  settings.nTime = -1;

  //请求的品种。DATA_TYPE_ALL请求所有品种
  settings.nTypeFlags = DATA_TYPE_NONE; 
  nErr = TDF_ERR_SUCCESS;

  if ((nTDF = TDF_Open(&settings, &nErr)) == NULL)
    LOG(ERROR) << "TDF_Open return error: " << nErr;
  else
    LOG(INFO) << " Open Success!";

  return 0;
}

int WindMarket::close() {
  closeFlag = true;
  TDF_Close(nTDF);

  return 0;
}
