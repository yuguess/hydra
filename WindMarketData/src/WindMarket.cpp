#include "ProtoBufMsgHub.h"

WindMarket::WindMarket() {
  TDF_OPEN_SETTING settings = {0};
  closeFlag = false;

  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(onMsg);
}

WindMarket::~WindMarket() {
  if (!closeFlag)
    close();
}

int WindMarket::start() {
  strcpy(settings.szIp, svr_ip);
  sprintf(settings.szPort, "%d", svr_port);
  strcpy(settings.szUser, "TD1000961001");
  strcpy(settings.szPwd,  "41376499");

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

  //"600030.SH"; 600030.SH;104174.SH;103493.SH";
  //需要订阅的股票,为空则订阅全市场
  settings.szSubScriptions = "000001.SZ;000002.SH";

  //请求的日期，格式YYMMDD，为0则请求今天
  //请求的时间，格式HHMMSS，为0则请求实时行情，为0xffffffff从头请求
  //settings.nDate = 0;
  settings.nTime = -1;

  //请求的品种。DATA_TYPE_ALL请求所有品种
  settings.nTypeFlags = DATA_TYPE_NONE; 
  TDF_ERR nErr = TDF_ERR_SUCCESS;
  THANDLE hTDF = NULL;

  hTDF = TDF_OpenExt(&settings, &nErr);

  if (hTDF == NULL)
    LOG(ERROR) << "TDF_Open return error: " << nErr;
  else
    LOG(INFO) << " Open Success!";

  return 0;
}

int WindMarket::close() {
  return 0;
}

int boardcastMsgData() {
  //called market data 
  return 0;
}

int onMsg() {
  //register new market data in interface
  return 0;
}

void RecvSys(THANDLE hTdf, TDF_MSG* pSysMsg) {
  if (!pSysMsg || !hTdf) {
    return;
  }

  switch (pSysMsg->nDataType) {
  case MSG_SYS_DISCONNECT_NETWORK:
    LOG(INFO) << "MSG_SYS_DISCONNECT_NETWORK";
    break;

  case MSG_SYS_CONNECT_RESULT: {
    TDF_CONNECT_RESULT* pConnResult = (TDF_CONNECT_RESULT*)pSysMsg->pData;
    if (pConnResult && pConnResult->nConnResult)
      LOG(INFO) << "connect: " << pConnResult->szIp << ":" 
        << pConnResult->szPort << " user:%s" << pConnResult->szUser << "suc!";
    else
      LOG(ERROR) << "connect: " << pConnResult->szIp << ":" 
        << pConnResult->szPort << " user:%s" << pConnResult->szUser << "fail !";
  }
  break;

  case MSG_SYS_LOGIN_RESULT: {
    TDF_LOGIN_RESULT* pLoginResult = (TDF_LOGIN_RESULT*)pSysMsg->pData;
    if (pLoginResult && pLoginResult->nLoginResult) {
      LOG(INFO) << "login suc:info:" << pLoginResult->szInfo
        << " nMarkets " << pLoginResult->nMarkets;
      for (int i=0; i<pLoginResult->nMarkets; i++) {
        printf("market:%s, dyn_date:%d\n", pLoginResult->szMarket[i], 
            pLoginResult->nDynDate[i]);
      }
    } else
      LOG(ERROR) << "login fail: " <<  pLoginResult->szInfo;
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
