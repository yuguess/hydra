#include "TDFAPI.h"
#include "ProtoBufMsgHub.h"
#include "WindDataCollector.h"
#include "ctime"

#define ELEM_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))
#define SAFE_STR(str) ((str)?(str):"")
#define SAFE_CHAR(ch) ((ch) ? (ch) : ' ')

std::string WindDataCollector::targetDir;
std::set<std::string> WindDataCollector::CodeRecorder;

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

void intarr2Jsonarr(int arr[], int n, Json::Value &JsonArr) {
  for (int i = 0; i < n; i++) {
    JsonArr.append(std::to_string(arr[i]));
  }
}
void chararr2Jsonarr(char arr[], int n, Json::Value &JsonArr) {
  for(int i = 0; i < n; i++) {
    JsonArr.append(std::string(&arr[i]));
  }
}

bool fileExist(const char* fileName) {
  std::ifstream infile(fileName);
  return infile.good();
}

#define GETRECORD(pBase, TYPE, nIndex) ((TYPE*)((char*)(pBase) + sizeof(TYPE)*(nIndex)))
#define PRINTNUM  1
static int recordNum = 0;
void WindDataCollector::RecvData(THANDLE hTdf, TDF_MSG* pMsgHead) {
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
        DumpMarketToLog((TDF_MARKET_DATA*)pMsgHead->pData, nItemCount);
      }
    }
    break;

    case MSG_DATA_FUTURE: {
      assert(nItemSize == sizeof(TDF_FUTURE_DATA));
      if (recordNum > PRINTNUM) {
        recordNum = 0;
        //DumpFutureToLog(hTdf,(TDF_FUTURE_DATA*)pMsgHead->pData, nItemCount);
      }
      TDF_FUTURE_DATA* pLastFuture = GETRECORD(pMsgHead->pData, 
          TDF_FUTURE_DATA, nItemCount-1);
      printf( "FutureData: szWindCode：%s, nActionDay:%d, nTime:%d, nMatch:%d, \
         iOpenInterest:%lld \n", pLastFuture->szWindCode, pLastFuture->nActionDay, 
         pLastFuture->nTime, pLastFuture->nMatch, pLastFuture->iOpenInterest);
    }
    break;

    case MSG_DATA_INDEX: {
      if (recordNum > PRINTNUM){
        recordNum = 0;
        //DumpIndexToLog((TDF_INDEX_DATA*)pMsgHead->pData, nItemCount);
      }

      TDF_INDEX_DATA* pLastIndex = GETRECORD(pMsgHead->pData,TDF_INDEX_DATA,
          nItemCount - 1);
      printf( "IndexData: szWindCode：%s, nActionDay:%d, nTime:%d, nLastIndex:%d, \
          iTotalVolume:%lld \n", pLastIndex->szWindCode, pLastIndex->nActionDay, 
          pLastIndex->nTime, pLastIndex->nLastIndex, pLastIndex->iTotalVolume);
    }
    break;

    case MSG_DATA_TRANSACTION: {
      if (recordNum > PRINTNUM) {
        recordNum = 0;
        //DumpTransToLog((TDF_TRANSACTION*)pMsgHead->pData, nItemCount);
      }
      TDF_TRANSACTION* pLastTransaction = GETRECORD(pMsgHead->pData,
          TDF_TRANSACTION, nItemCount-1);
      printf( "Transaction: szWindCode：%s, nActionDay:%d, nTime:%d, \
          nTime:%d, nPrice:%d \n", pLastTransaction->szWindCode,
        pLastTransaction->nActionDay, pLastTransaction->nTime, 
        pLastTransaction->nPrice, pLastTransaction->nVolume);
    }
    break;

    case MSG_DATA_ORDERQUEUE: {
      if (recordNum > PRINTNUM){
        recordNum = 0;
        //DumpOrderQueueToLog((TDF_ORDER_QUEUE*)pMsgHead->pData, nItemCount);
      }

      TDF_ORDER_QUEUE* pLastOrderQueue = GETRECORD(pMsgHead->pData,
          TDF_ORDER_QUEUE, nItemCount-1);
      printf( "OrderQueue: szWindCode：%s, nActionDay:%d, nTime:%d, \
        nPrice:%d, nOrders:%d \n", pLastOrderQueue->szWindCode, 
        pLastOrderQueue->nActionDay, pLastOrderQueue->nTime, 
        pLastOrderQueue->nPrice, pLastOrderQueue->nOrders);
    }
    break;

    case MSG_DATA_ORDER: {
      if (recordNum > PRINTNUM){
        recordNum = 0;
        //DumpOrderToLog((TDF_ORDER*)pMsgHead->pData, nItemCount);
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

      printf("Order: szWindCode：%s, nActionDay:%d, nTime:%d, nPrice:%d,\
        nVolume:%d \n", pLastOrder->szWindCode, pLastOrder->nActionDay, 
      pLastOrder->nTime, pLastOrder->nPrice, pLastOrder->nVolume);
    }
    break;

    default:
    assert(0);
    break;
  }
}

void WindDataCollector::RecvSys(THANDLE hTdf, TDF_MSG* pSysMsg) {
  if (!pSysMsg ||! hTdf) {
    return;
  }

  switch (pSysMsg->nDataType) {
  case MSG_SYS_DISCONNECT_NETWORK:
    printf("MSG_SYS_DISCONNECT_NETWORK\n");
    break;

  case MSG_SYS_CONNECT_RESULT: {
    TDF_CONNECT_RESULT* pConnResult = (TDF_CONNECT_RESULT*)pSysMsg->pData;
    if (pConnResult && pConnResult->nConnResult)
      printf("connect: %s:%s user:%s, password:%s suc!\n", pConnResult->szIp, 
          pConnResult->szPort, pConnResult->szUser, pConnResult->szPwd);
    else
      printf("connect: %s:%s user:%s, password:%s fail!\n", pConnResult->szIp, 
          pConnResult->szPort, pConnResult->szUser, pConnResult->szPwd);
  }
  break;

  case MSG_SYS_LOGIN_RESULT: {
    TDF_LOGIN_RESULT* pLoginResult = (TDF_LOGIN_RESULT*)pSysMsg->pData;
    if (pLoginResult && pLoginResult->nLoginResult) {
      printf("login suc:info:%s, nMarkets:%d\n", pLoginResult->szInfo, 
          pLoginResult->nMarkets);
      for (int i=0; i<pLoginResult->nMarkets; i++) {
        printf("market:%s, dyn_date:%d\n", pLoginResult->szMarket[i], 
            pLoginResult->nDynDate[i]);
      }

    } else
      printf("login fail:%s\n", pLoginResult->szInfo);

  }
  break;

  case MSG_SYS_CODETABLE_RESULT: {
    TDF_CODE_RESULT* pCodeResult = (TDF_CODE_RESULT*)pSysMsg->pData;
    if (pCodeResult) {
      printf("get codetable:info:%s, num of market:%d\n", pCodeResult->szInfo, 
          pCodeResult->nMarkets);
      for (int i=0; i<pCodeResult->nMarkets; i++) {
        printf("market:%s, codeCount:%d, codeDate:%d\n", 
            pCodeResult->szMarket[i], pCodeResult->nCodeCount[i], 
            pCodeResult->nCodeDate[i]);

        if (1) {
          //CodeTable
          TDF_CODE* pCodeTable; 
          unsigned int nItems;
          TDF_GetCodeTable(hTdf, pCodeResult->szMarket[i], &pCodeTable, &nItems);
          printf("nItems =%d\n",nItems);
          for (int i=0; i < nItems; i++) {
            TDF_CODE& code = pCodeTable[i];
            printf("windcode:%s, code:%s, market:%s, name:%s, nType:0x%x\n",
                code.szWindCode, code.szCode, code.szMarket, code.szCNName, 
                code.nType);
          }
          TDF_FreeArr(pCodeTable);
        }
      }
    }
  }
  break;

  case MSG_SYS_QUOTATIONDATE_CHANGE: {
    TDF_QUOTATIONDATE_CHANGE* pChange = 
      (TDF_QUOTATIONDATE_CHANGE*)pSysMsg->pData;
    if (pChange) {
      printf("MSG_SYS_QUOTATIONDATE_CHANGE: market:%s, nOldDate:%d, \
          nNewDate:%d\n", pChange->szMarket, pChange->nOldDate, 
          pChange->nNewDate);
    }
  }
  break;

  case MSG_SYS_MARKET_CLOSE: {
    TDF_MARKET_CLOSE* pCloseInfo = (TDF_MARKET_CLOSE*)pSysMsg->pData;
    if (pCloseInfo) {
      printf("MSG_SYS_MARKET_CLOSE\n");
    }
  }
  break;

  case MSG_SYS_HEART_BEAT:
    printf("MSG_SYS_HEART_BEAT...............\n");
  break;

  default:
    assert(0);
    break;
  }
}

void WindDataCollector::DumpMarketToLog(TDF_MARKET_DATA* pMarket, int nItems) {
  char szBuf1[512];
  char szBuf2[512];
  char szBuf3[512];
  char szBuf4[512];
  char szBufSmall[64];

  for (int i=0; i<nItems; i++) {
    const TDF_MARKET_DATA& marketData = pMarket[i];

    std::string targetFile = targetDir + marketData.szWindCode + "/MD";
    struct stat sb;
    if(stat((targetDir + marketData.szWindCode).c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
    else {
      if(CodeRecorder.find(marketData.szWindCode) == CodeRecorder.end()) {
        if(mkdir((targetDir + marketData.szWindCode).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0)
          CodeRecorder.insert(marketData.szWindCode);
        else 
          LOG(WARNING) << "mkdir code folder failed!";
      }
    }

    Json::Value root;
    std::string windCode = marketData.szWindCode;
    root[windCode]["szCode"] = marketData.szCode;
    root[windCode]["nActionDay"] = marketData.nActionDay;
    root[windCode]["nTradingDay"] = marketData.nTradingDay;
    root[windCode]["nTime"] = marketData.nTime;
    root[windCode]["nStatus"] = marketData.nStatus;
    root[windCode]["nPreClose"] = marketData.nPreClose;
    root[windCode]["nOpen"] = marketData.nOpen;
    root[windCode]["nHigh"] = marketData.nHigh;
    root[windCode]["nLow"] = marketData.nLow;
    root[windCode]["nMatch"] = marketData.nMatch;
    Json::Value AskPrice;
    intarr2Jsonarr((int*)marketData.nAskPrice, ELEM_COUNT(marketData.nAskPrice), AskPrice);
    root[windCode]["nAskPrice"] = AskPrice;
    Json::Value AskVol;
    intarr2Jsonarr((int*)marketData.nAskVol, ELEM_COUNT(marketData.nAskVol), AskVol);
    root[windCode]["nAskVol"] = AskVol;
    Json::Value BidPrice;
    intarr2Jsonarr((int*)marketData.nBidPrice, ELEM_COUNT(marketData.nBidPrice), BidPrice);
    root[windCode]["nBidPrice"] = BidPrice;
    Json::Value BidVol;
    intarr2Jsonarr((int*)marketData.nBidVol, ELEM_COUNT(marketData.nBidVol), BidVol);
    root[windCode]["nBidVol"] = BidVol;
    root[windCode]["nNumTrades"] = marketData.nNumTrades;
    root[windCode]["iVolume"] = marketData.iVolume;
    root[windCode]["nTotalBidVol"] = marketData.nTotalBidVol;
    root[windCode]["nTotalAskVol"] = marketData.nTotalAskVol;
    root[windCode]["nWeightedAvgBidPrice"] = marketData.nWeightedAvgBidPrice;
    root[windCode]["nWeightedAvgAskPrice"] = marketData.nWeightedAvgAskPrice;
    root[windCode]["nIOPV"] = marketData.nIOPV;
    root[windCode]["nYieldToMaturity"] = marketData.nYieldToMaturity;
    root[windCode]["nHighLimited"] = marketData.nHighLimited;
    root[windCode]["nLowLimited"] = marketData.nLowLimited;
    Json::Value Prefix;
    chararr2Jsonarr((char*)marketData.chPrefix, ELEM_COUNT(marketData.chPrefix), Prefix);
    root[windCode]["chPrefix"] = Prefix;
    root[windCode]["nSyl1"] = marketData.nSyl1;
    root[windCode]["nSyl2"] = marketData.nSyl2;
    root[windCode]["nSD2"] = marketData.nSD2;

    std::ofstream loggingFile;
    loggingFile.open(targetFile, std::ios_base::app); 
    loggingFile << root;
  }
}

void WindDataCollector::DumpIndexToLog(TDF_INDEX_DATA* pIndex, int nItems) {
  LOG(INFO) << "-------- Index, Count: " << nItems << "--------"; 

  for (int i=0; i<nItems; i++) {
    const TDF_INDEX_DATA& indexData = pIndex[i];
    LOG(INFO) << "szWindCode: " << indexData.szWindCode;
    LOG(INFO) << "szCode: " << indexData.szCode;
    LOG(INFO) << "nActionDay: " << indexData.nActionDay;
    LOG(INFO) << "nTradingDay: " << indexData.nTradingDay;
    LOG(INFO) << "nTime: " << indexData.nTime;
    LOG(INFO) << "nOpenIndex: " << indexData.nOpenIndex;
    LOG(INFO) << "nHighIndex: " << indexData.nHighIndex;
    LOG(INFO) << "nLowIndex: " << indexData.nLowIndex;
    LOG(INFO) << "nLastIndex: " << indexData.nLastIndex;
    LOG(INFO) << "iTotalVolume: " << indexData.iTotalVolume;
    LOG(INFO) << "iTurnover: " << indexData.iTurnover;
    LOG(INFO) << "nPreCloseIndex: " << indexData.nPreCloseIndex;
  }
}

void WindDataCollector::DumpTransToLog(TDF_TRANSACTION* pTrans, int nItems) {
}

WindDataCollector::WindDataCollector() {
  closeFlag = false;
}

WindDataCollector::~WindDataCollector() {
  if (!closeFlag)
    close();
}

int WindDataCollector::start() {
  char datebuffer[80];
  time_t rawtime;
  struct tm* timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(datebuffer, 80, "%Y%m%d", timeinfo);
  WindDataCollector::targetDir = "./WindDataRepo/" + std::string(datebuffer) + "/";
   
  struct stat sb;
  if(stat(targetDir.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode));
  else {
    mkdir("./WindDataRepo/", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(mkdir(WindDataCollector::targetDir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
      LOG(WARNING) << "mkdir WindDataRepo failed!";
      exit(-1);
    }
  }

  el::Configurations conf("../config/WindMD.conf");
  el::Loggers::reconfigureLogger("WindMD", conf);

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
  settings.szSubScriptions = "000001.SH";

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

int WindDataCollector::close() {
  closeFlag = true;
  TDF_Close(nTDF);

  return 0;
}
