#include "TDFAPI.h"
#include "ProtoBufMsgHub.h"
#include "WindMarket.h"

//extern WindMarket windMarket;

/* void RelayMarket(TDF_MARKET_DATA* pMarket, int nItems); */
/* void RelayFuture(THANDLE hTdf,TDF_FUTURE_DATA* pFuture, int nItems); */
/* void RelayIndex(TDF_INDEX_DATA* pIndex, int nItems); */
/* void RelayTransaction(TDF_TRANSACTION* pTransaction, int nItems); */
/* void RelayOrder(TDF_ORDER* pOrder, int nItems); */
/* void RelayOrderQueue(TDF_ORDER_QUEUE* pOrderQueue, int nItems); */
void DumpScreenMarket(TDF_MARKET_DATA*, int);
void DumpScreenIndex(TDF_INDEX_DATA*, int);

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

ProtoBufMsgHub WindMarket::msgHub;

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
        RelayMarket((TDF_MARKET_DATA*)pMsgHead->pData, nItemCount);
        DumpScreenMarket((TDF_MARKET_DATA*)pMsgHead->pData, nItemCount);
      }
    }
    break;

    case MSG_DATA_FUTURE: {
      assert(nItemSize == sizeof(TDF_FUTURE_DATA));
      if (recordNum > PRINTNUM) {
        recordNum = 0;
        //DumpScreenFuture(hTdf,(TDF_FUTURE_DATA*)pMsgHead->pData, nItemCount);
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
        RelayIndexData((TDF_INDEX_DATA*)pMsgHead->pData, nItemCount);
        DumpScreenIndex((TDF_INDEX_DATA*)pMsgHead->pData, nItemCount);
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
    	//RelayTransaction((TDF_TRANSACTION*)pMsgHead->pData, nItemCount);
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

void DumpScreenMarket(TDF_MARKET_DATA* pMarket, int nItems) {

  printf("-------- Market, Count:%d --------\n", nItems);
  char szBuf1[512];
  char szBuf2[512];
  char szBuf3[512];
  char szBuf4[512];
  char szBufSmall[64];

  for (int i=0; i<nItems; i++) {
    const TDF_MARKET_DATA& marketData = pMarket[i];
    printf("szWindCode: %s\n", marketData.szWindCode);
    printf("szCode: %s\n", marketData.szCode);
    printf("nActionDay: %d\n", marketData.nActionDay);
    printf("nTradingDay: %d\n", marketData.nTradingDay);

    printf("nTime: %d\n", marketData.nTime );
    printf("nStatus: %d(%c)\n", marketData.nStatus, 
        SAFE_CHAR(marketData.nStatus));
    printf("nPreClose: %d\n", marketData.nPreClose);
    printf("nOpen: %d\n", marketData.nOpen);
    printf("nHigh: %d\n", marketData.nHigh);
    printf("nLow: %d\n", marketData.nLow);
    printf("nMatch: %d\n", marketData.nMatch);
    printf("nAskPrice: %s \n", 
      intarr2str(szBuf1, sizeof(szBuf1), (int*)marketData.nAskPrice, 
      ELEM_COUNT(marketData.nAskPrice)));

    printf("nAskVol: %s \n", intarr2str(szBuf2, sizeof(szBuf2), 
          (int*)marketData.nAskVol, ELEM_COUNT(marketData.nAskVol)));
    printf("nBidPrice: %s \n", intarr2str(szBuf3, sizeof(szBuf3), 
          (int*)marketData.nBidPrice, ELEM_COUNT(marketData.nBidPrice)));
    printf("nBidVol: %s \n", intarr2str(szBuf4, sizeof(szBuf4), 
          (int*)marketData.nBidVol, ELEM_COUNT(marketData.nBidVol)));
    printf("nNumTrades: %d\n", marketData.nNumTrades);

    printf("iVolume: %lld\n", marketData.iVolume);
    printf("iTurnover: %lld\n", marketData.iTurnover);
    printf("nTotalBidVol: %lld\n", marketData.nTotalBidVol);
    printf("nTotalAskVol: %lld\n", marketData.nTotalAskVol);

    printf("nWeightedAvgBidPrice: %u\n", marketData.nWeightedAvgBidPrice);
    printf("nWeightedAvgAskPrice: %u\n", marketData.nWeightedAvgAskPrice);

    printf("nIOPV: %d\n",  marketData.nIOPV);
    printf("nYieldToMaturity: %d\n", marketData.nYieldToMaturity);
    printf("nHighLimited: %d\n", marketData.nHighLimited);
    printf("nLowLimited: %d\n", marketData.nLowLimited);
    printf("chPrefix: %s\n", chararr2str(szBufSmall, sizeof(szBufSmall), 
          (char*)marketData.chPrefix, ELEM_COUNT(marketData.chPrefix)));
    printf("nSy11: %d\n", marketData.nSyl1);
    printf("nSy12: %d\n", marketData.nSyl2);
    printf("nSD2: %d\n", marketData.nSD2);
    if (nItems>1)
      printf("\n");
  }

  printf("\n");
}

void DumpScreenIndex(TDF_INDEX_DATA* pIndex, int nItems) {
  printf("-------- Index, Count:%d --------\n", nItems);

  for (int i=0; i<nItems; i++) {
    const TDF_INDEX_DATA& indexData = pIndex[i];
    printf("万得代码 szWindCode: %s\n", indexData.szWindCode);
    printf("原始代码 szCode: %s\n", indexData.szCode);
    printf("业务发生日(自然日) nActionDay: %d\n", indexData.nActionDay);
    printf("交易日 nTradingDay: %d\n", indexData.nTradingDay);
    printf("时间(HHMMSSmmm) nTime: %d\n", indexData.nTime);

    printf("今开盘指数 nOpenIndex: %d\n", indexData.nOpenIndex);
    printf("最高指数 nHighIndex: %d\n", indexData.nHighIndex);
    printf("最低指数 nLowIndex: %d\n", indexData.nLowIndex);
    printf("最新指数 nLastIndex: %d\n", indexData.nLastIndex);
    printf("成交总量 iTotalVolume: %lld\n", indexData.iTotalVolume);
    printf("成交总金额 iTurnover: %lld\n", indexData.iTurnover);
    printf("前盘指数 nPreCloseIndex: %d\n", indexData.nPreCloseIndex);

    if (nItems>1)
      printf("\n");
  }

  printf("\n");
}

void WindMarket::RelayMarket(TDF_MARKET_DATA* pMarket, int nItems) {

  MarketUpdate mktUpdt;
  for (int i = 0; i < nItems; i++) {
    const TDF_MARKET_DATA& marketData = pMarket[i];
    const char* exchange = marketData.szWindCode + 7;

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

    for (int j = 0; j < priceLevel; j++) {
      mktUpdt.add_bid_price(marketData.nBidPrice[j]);
      mktUpdt.add_bid_volume(marketData.nBidVol[j]);
      mktUpdt.add_ask_price(marketData.nAskPrice[j]);
      mktUpdt.add_ask_volume(marketData.nAskVol[j]);
    }
    std::string res = 
      ProtoBufHelper::wrapMsg<MarketUpdate>(TYPE_MARKETUPDATE, mktUpdt); 
    if (msgHub.boardcastMsg(marketData.szCode, res) != 0) 
      LOG(INFO) << "relay mktUpdt successfully!";
    else 
      LOG(WARNING) << "relay mktUpdt failed!";
  }   

}

void WindMarket::RelayTransaction(TDF_TRANSACTION* pMarket, int nItems) {
  Transaction trans;
  for (int i = 0; i < nItems; i++) {
    const TDF_TRANSACTION& tdfTrans = pMarket[i];
    const char* exchange = tdfTrans.szWindCode + 7;

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
    if (msgHub.boardcastMsg(tdfTrans.szCode, res) != 0) 
      LOG(INFO) << "relay trans successfully!";
    else 
      LOG(WARNING) << "relay trans failed!";
  }
}

void WindMarket::RelayIndexData(TDF_INDEX_DATA* pIndex, int nItems) {
  IndexData indexd;
  for (int i = 0; i < nItems; i++) {
    const TDF_INDEX_DATA& tdfIndex = pIndex[i];
    const char* exchange = tdfIndex.szWindCode + 7;

    indexd.set_code(tdfIndex.szCode);
    indexd.set_symbol(tdfIndex.szWindCode);
    indexd.set_exchange(exchange);
    indexd.set_action_day(tdfIndex.nActionDay);
    indexd.set_time(tdfIndex.nTime);
    indexd.set_open_index(tdfIndex.nOpenIndex);
    indexd.set_high_index(tdfIndex.nHighIndex);
    indexd.set_low_index(tdfIndex.nLowIndex);
    indexd.set_last_index(tdfIndex.nLastIndex);
    indexd.set_total_volume(tdfIndex.iTotalVolume);
    indexd.set_pre_close_index(tdfIndex.nPreCloseIndex);

    std::string res = 
      ProtoBufHelper::wrapMsg<IndexData>(TYPE_INDEX_DATA, indexd);

    if (msgHub.boardcastMsg(tdfIndex.szCode, res) != 0) 
      LOG(INFO) << "relay indexd successfully!";
    else 
      LOG(WARNING) << "relay indexd failed!";
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
  //settings.szMarkets = "SZ-2;";
  settings.szMarkets = "SZ-2;SH-2";
  //settings.szMarkets = "SZ;SH";

  //需要订阅的股票,为空则订阅全市场
  settings.szSubScriptions = "";

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
