#include "TradeHandler.h"
#include "CedarHelper.h"
//#include "MarketUpdate.h"

TradeHandler::TradeHandler() {
  pUserTradeApi = CThostFtdcTraderApi::CreateFtdcTraderApi();

  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(std::bind(&TradeHandler::onMsg, 
        this, std::placeholders::_1));
}

int TradeHandler::start() {
  std::string tradeFront;
  CedarJsonConfig::getInstance().getStringByPath("CTP.TradeFront", tradeFront);
  LOG(INFO) << tradeFront;
  pUserTradeApi->RegisterSpi(this);

  pUserTradeApi->SubscribePrivateTopic(THOST_TERT_QUICK);
  pUserTradeApi->SubscribePublicTopic(THOST_TERT_QUICK);

  pUserTradeApi->RegisterFront(const_cast<char*>(tradeFront.c_str()));
  pUserTradeApi->Init();
}

void TradeHandler::OnFrontConnected() {
  LOG(INFO) << "FrontConnected";
  
  CedarJsonConfig::getInstance().getStringByPath("CTP.UserID", userId);
  CedarJsonConfig::getInstance().getStringByPath("CTP.BrokerID", brokerId);
  CedarJsonConfig::getInstance().getStringByPath("CTP.Password", password);

  CThostFtdcReqUserLoginField req;
  strcpy(req.BrokerID, brokerId.c_str());
  strcpy(req.UserID, userId.c_str());
  strcpy(req.Password, password.c_str());

  int result = pUserTradeApi->ReqUserLogin(&req, 0);
}

void TradeHandler::OnRspUserLogin(
    CThostFtdcRspUserLoginField *pRspUserLogin, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

  if (pRspInfo->ErrorID != 0) {
    // fail to login !!!
    LOG(ERROR) << "Failed to login !!!";
    LOG(ERROR) << "Login ErrorCode:" << pRspInfo->ErrorID
              << " ErrorMsg:" << pRspInfo->ErrorMsg
              << " RequestID:" << nRequestID
              << " Chain:" << bIsLast;
    exit(-1);
  }

  LOG(INFO) <<">>>>>>> China Future Trade API: Login Successfully";

  //memcpy(&g_Login, pRspUserLogin, sizeof(CThostFtdcRspUserLoginField));
  frontID = pRspUserLogin->FrontID;
  sessionID = pRspUserLogin->SessionID;

  CThostFtdcSettlementInfoConfirmField pSettlementConfirm;
  strcpy(pSettlementConfirm.BrokerID, brokerId.c_str());
  strcpy(pSettlementConfirm.InvestorID, userId.c_str());
  pUserTradeApi->ReqSettlementInfoConfirm(&pSettlementConfirm,0);
}

int TradeHandler::onMsg(MessageBase msg) {
  if (msg.type() == TYPE_ORDER_REQUEST) {
    OrderRequest orderReq = ProtoBufHelper::unwrapMsg<OrderRequest>(msg);
    //LOG(INFO) << "recv data request " << dataReq.code() << " into codes";
  } else {
    LOG(WARNING) << "recv invalid msg type " << msg.type();
  }

  return 0;
}

int TradeHandler::initReq(CThostFtdcInputOrderField &req) {
  memset(&req, 0, sizeof(req));

  strcpy(req.BrokerID, brokerId.c_str());
  strcpy(req.InvestorID, userId.c_str());
  strcpy(req.UserID, userId.c_str());

  //成交量类型: 任何数量
	req.VolumeCondition = THOST_FTDC_VC_AV;
	//最小成交量: 1
	req.MinVolume = 1;
  ///强平原因: 非强平
	req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
  ///自动挂起标志: 否
	req.IsAutoSuspend = 0;
	///用户强评标志: 否
	req.UserForceClose = 0;

  return 0;
}


int TradeHandler::sendOrderReq(OrderRequest &req) {

  if (req.type() == TYPE_LIMIT_ORDER_REQUEST ||
      req.type() == TYPE_MARKET_ORDER_REQUEST) {

    int reqId = getIncreaseID();
    CThostFtdcInputOrderField ctpReq;
    initReq(ctpReq);
    strcpy(ctpReq.InstrumentID, req.code().c_str());
    
    sprintf(ctpReq.OrderRef, "%12d", reqId);

    ctpReq.Direction = (req.buy_sell() == LONG_BUY ? 
      THOST_FTDC_D_Buy : THOST_FTDC_D_Sell); 

    //组合投机套保标志
	  ctpReq.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
	  ctpReq.VolumeTotalOriginal = req.trade_quantity();

    if (req.open_close() == OPEN_POSITION)
      ctpReq.CombOffsetFlag[0]  = THOST_FTDC_OF_Open;
    else if (req.open_close() == CLOSE_POSITION) 
      ctpReq.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
    else if (req.open_close() == CLOSE_YESTERDAY_POSITION) 
      ctpReq.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;
    else  
      LOG(ERROR) << "Recv Invalid open_close in new order request";
  
    if (req.type() == TYPE_LIMIT_ORDER_REQUEST) {
      ctpReq.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
      ctpReq.TimeCondition = THOST_FTDC_TC_GFD;
      ctpReq.LimitPrice = req.limit_price();
      LOG(INFO) << "recv new limit order";
    } else if (req.type() == TYPE_MARKET_ORDER_REQUEST) {
      ctpReq.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
      ctpReq.TimeCondition = THOST_FTDC_TC_GFD;
      ctpReq.LimitPrice = 0;
    }

    pUserTradeApi->ReqOrderInsert(&ctpReq, 0);

    //record in idmap
    std::string internalId = req.response_address() + "_" + req.id();
    internalIdToExternal[internalId] = reqId;  
    externalIdToInternal[reqId] = internalId;
    LOG(INFO) << "recv new order req internal id " << internalId
              << "external id" << reqId;

  } else if (req.type() == TYPE_CANCEL_ORDER_REQUEST) {
    int reqId = getIncreaseID();
    CThostFtdcInputOrderActionField cnclReq;
    memset(&cnclReq, 0, sizeof(cnclReq));

    cnclReq.OrderActionRef = reqId;
    cnclReq.ActionFlag = THOST_FTDC_AF_Delete;

    cnclReq.FrontID = frontID;
    cnclReq.SessionID = sessionID;

    std::string internalId = req.response_address() + "_" 
      + req.cancel_order_id();
    if (internalIdToExternal.find(internalId) != 
        internalIdToExternal.end()) {  
      sprintf(cnclReq.OrderRef, "%12d", internalIdToExternal[internalId]);
    } else {
    //TODO serisouly error !!!
      LOG(ERROR) << "Try to cancel an order with internal id"
        << internalId << " , but can't locate the external id";
      return -1;
    }

    //ExchangeID
    //OrderSysID

    strcpy(cnclReq.BrokerID, brokerId.c_str());
    strcpy(cnclReq.UserID, userId.c_str());
    strcpy(cnclReq.InvestorID, userId.c_str());
    strcpy(cnclReq.InstrumentID, req.code().c_str());  


    //printf("send cancel request\n");
    //pUserTradeApi->ReqOrderAction(&cancelReq, 1);

  } else {
    LOG(ERROR) << "recv an invalid order req type" << req.type(); 
    return -1;
  }

   
  

  //enter map
  //orderRef

  //req enter map
  
  ////报单价格条件: 限价
  //if (order.type == LIMIT) {
  //  ctpReq.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
  //  printf("Limit order\n");
  //  //价格
  //  ctpReq.LimitPrice = order.price;
  //} else if (order.type == MARKET) {

  //} else {
  //  //TODO use log to record 
  //}

  ////组合开平标志: 开仓
  //ctpReq.CombOffsetFlag[0] = (order.orderPosition);
  ////组合投机套保标志
  //ctpReq.CombHedgeFlag[0] = '1';

  ////买卖方向:
  //if (order.side == Direction::BUY) {
  //  ctpReq.Direction = THOST_FTDC_D_Buy;
  //} else if (order.side == Direction::SELL) {
  //  ctpReq.Direction = THOST_FTDC_D_Sell;
  //}
  ////数量
  //ctpReq.VolumeTotalOriginal = order.qty;

  //std::string ctpOrderRef = concatOrderRef(order.id); 
  //strcpy(ctpReq.OrderRef, ctpOrderRef.c_str()); 
  //ctpReq.RequestID = getIncreaseID();

  //printf("CTP reqID %d, OrderRef %s, Direction %s, Qty %d, Price %f, Broker %s,"
  //    "Investor %s, UserID %s, InstrumentID %s\n", 
  //    ctpReq.RequestID, ctpReq.OrderRef,
  //    ctpReq.Direction == THOST_FTDC_D_Buy ? "BUY":"SELL", ctpReq.VolumeTotalOriginal, 
  //    ctpReq.LimitPrice, ctpReq.BrokerID, ctpReq.InvestorID, ctpReq.UserID, ctpReq.InstrumentID);

  return 0;
}

//THost check error, this function will be called !!!
void TradeHandler::OnRspOrderInsert(
    CThostFtdcInputOrderField *pInputOrder, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  
  //LOG(ERROR) << __FUNCTION__ << std::endl;
  //returnErrorInfo(pRspInfo);

  //fn_printCout(__FUNCTION__);
  //LOG(ERROR) << << ;

  //OnRspOrderInsertMsg msg;
  //memcpy(&msg.InputOrder, pInputOrder, sizeof(CThostFtdcInputOrderField));
  //memcpy(&msg.RspInfo, pRspInfo, sizeof(CThostFtdcRspInfoField));
  //msg.RequestID = nRequestID;
  //msg.IsLast = bIsLast;

  //SendMsg(TD_OnRspOrderInsert, (char*)&msg);
}


void TradeHandler::OnRtnOrder(CThostFtdcOrderField *pOrder) {
  //LOG(INFO) << "CTP OnRtnORder| " <<  "reqID:" << pOrder->RequestID << ",OrderRef:" <<  pOrder->OrderRef
  //           << ",Direction:" << (pOrder->Direction == THOST_FTDC_D_Buy ? "BUY":"SELL") 
  //           << ",OrderStatus:" << pOrder->OrderStatus << ",SubmitStatus:" 
  //           << pOrder->OrderSubmitStatus << std::endl; 

  //std::string chan = C_CTPTradeReturn;

  //ReturnInfo rtnInfo;
  //rtnInfo.id = extractInternalID(pOrder->OrderRef);
  //rtnInfo.symbol = std::string(pOrder->InstrumentID);
  //rtnInfo.price = pOrder->LimitPrice;
  //rtnInfo.qty = pOrder->VolumeTotalOriginal; 
  //if (pOrder->Direction == THOST_FTDC_D_Buy) {
  //  rtnInfo.side = Direction::BUY;  
  //} else if (pOrder->Direction == THOST_FTDC_D_Sell) {
  //  rtnInfo.side = Direction::SELL;
  //}

  //if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted && 
  //    pOrder->OrderStatus == THOST_FTDC_OST_NoTradeQueueing) {
  //  //NEW_CONFIRM
  //  rtnInfo.type = ReturnType::NEW_CONFIRM;
  //  std::string msg = rtnInfo.serialize(); 

  //  msgHub.send(chan, msg); 
  //  idMap[rtnInfo.id] = pOrder->OrderRef;

  //} else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted &&
  //    pOrder->OrderStatus == THOST_FTDC_OST_AllTraded) {

  //  idMap.erase(rtnInfo.id);
  //  
  //} else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted &&
  //    pOrder->OrderStatus == THOST_FTDC_OST_AllTraded) {

  //  if (pOrder->VolumeTotal == 0) {
  //    idMap.erase(rtnInfo.id);
  //  }
  //  
  //} else if ((pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted && 
  //  pOrder->OrderStatus == THOST_FTDC_OST_Canceled) || 
  //  (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted &&
  //  pOrder->OrderStatus == THOST_FTDC_OST_PartTradedNotQueueing)) { 

  //  //CANCEL_CONFIRM
  //  rtnInfo.type = ReturnType::CANCEL_CONFIRM;
  //  std::string tmpMsg = rtnInfo.serialize(); 
  //  msgHub.send(chan, tmpMsg); 

  //  //idMap delete;
  //  idMap.erase(rtnInfo.id);

  //}  else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected 
  //    || pOrder->OrderSubmitStatus == THOST_FTDC_OSS_CancelRejected 
  //    || pOrder->OrderSubmitStatus == THOST_FTDC_OSS_ModifyRejected) {

  //  rtnInfo.type = ReturnType::ERROR;

  //  if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected) {
  //    rtnInfo.msg = "CTPRequest Insert Reject";
  //  } else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_CancelRejected) {
  //    rtnInfo.msg = "CTPRequest Cancel Reject";
  //  } else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_ModifyRejected) {
  //    rtnInfo.msg = "CTPRequest Modify Reject";
  //  }
  //  
  //  std::string errorMsg = rtnInfo.serialize();
  //  msgHub.send(chan, errorMsg); 
  //}

  //LOG(INFO) << "OnRtnOrder send to OrderHub msg " << rtnInfo.serialize() << std::endl;
}


//int TradeHandler::sendCancelReq(NewOrderInfo &order) {
  //CThostFtdcInputOrderActionField cancelReq;
  //memset(&cancelReq, 0, sizeof(cancelReq));
  //strcpy(cancelReq.BrokerID, brokerId.c_str());
  //strcpy(cancelReq.InvestorID, userId.c_str());
  //strcpy(cancelReq.UserID, userId.c_str());

  //cancelReq.FrontID = frontID;
  //cancelReq.SessionID = sessionID;
  //cancelReq.ActionFlag = THOST_FTDC_AF_Delete;
  //cancelReq.LimitPrice = order.price;
  //cancelReq.RequestID = getIncreaseID();
  //cancelReq.VolumeChange = 0;

  //cancelReq.OrderActionRef = std::stoi(order.id);
  ////use idMap 
  //if (idMap.find(order.id) != idMap.end()) {  
  //  strcpy(cancelReq.OrderRef, idMap[order.id].c_str()); 
  //} else {
  //  //TODO serisouly error !!!
  //  printf("id not found, error !!!!!!!!!!\n");
  //  return -1;
  //}

  //strcpy(cancelReq.InstrumentID, order.symbol.c_str());

  //printf("send cancel request\n");
  //pUserTradeApi->ReqOrderAction(&cancelReq, 1);

  //return 0;
//}

std::string TradeHandler::concatOrderRef(std::string &internalID) {
  //static int orderRefID = 0;
  //char tmp[ORDER_DIGIT + 1];
  //++orderRefID;

  ////====ORDER_DIGIT====
  //sprintf(tmp, "%6d", orderRefID);

  //return std::string(tmp) + internalID; 
}

int TradeHandler::close() {
  //release
  return 0;
}



void TradeHandler::OnFrontDisconnected(int nReason) {
  //// 当发生这个情况后，API会自动重新连接，客户端可不做处理
  //fn_printCout(__FUNCTION__);
  //std::cout<<">>>>>>> Trade API Logged Out!"<<std::endl;
  LOG(WARNING) << "Front Disconnected"<<std::endl;
}

int TradeHandler::returnErrorInfo(CThostFtdcRspInfoField *pRspInfo) {
  //LOG(ERROR) << __FUNCTION__ << "ErrorCode: " 
  //  << pRspInfo->ErrorID << " ErrorMsg:" << pRspInfo->ErrorMsg << std::endl;

  //ReturnInfo rtnInfo;
  //rtnInfo.type = ReturnType::ERROR;
  //rtnInfo.msg = "ErrorID:" + std::to_string(pRspInfo->ErrorID) + 
  //  "," + std::string(pRspInfo->ErrorMsg);

  //std::string msg = rtnInfo.serialize();
  //std::string chan = C_CTPTradeReturn;
  //msgHub.send(chan, msg); 

  //return 0;
}



void TradeHandler::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo) {
  //LOG(ERROR) << __FUNCTION__ << std::endl;
  //returnErrorInfo(pRspInfo);

  //printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	//fn_printCout(__FUNCTION__);
}

void TradeHandler::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo) {
  //LOG(ERROR) << __FUNCTION__ << std::endl;
  //returnErrorInfo(pRspInfo);

	//printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	////fprintf(f_errorLog,"%d ErrorCode=[%d], ErrorMsg=[%s]\n", dataLength, pRspInfo->ErrorID, pRspInfo->ErrorMsg);
	////fflush(f_errorLog);
	//fn_printCout(__FUNCTION__);
}

void TradeHandler::OnRspParkedOrderInsert(
    CThostFtdcParkedOrderField *pParkedOrder, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

  //printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  //fn_printCout(__FUNCTION__);
}

//报单操作请求响应
void TradeHandler::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

  printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  //fprintf(f_errorLog,"%d ErrorCode=[%d], ErrorMsg=[%s]\n", dataLength, pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  //fflush(f_errorLog);
  //fn_printCout(__FUNCTION__);
}

///请求查询报单响应
void TradeHandler::OnRspQryOrder(CThostFtdcOrderField *pOrder, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    //fprintf(f_errorLog,"%d ErrorCode=[%d], ErrorMsg=[%s]\n", dataLength, pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    //fflush(f_errorLog);
    //fn_printCout(__FUNCTION__);
}

///请求查询成交响应
void TradeHandler::OnRspQryTrade(CThostFtdcTradeField *pTrade, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    //fn_printCout(__FUNCTION__);
}

///请求查询合约响应
void TradeHandler::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
//	if (pInstrument->ProductClass == THOST_FTDC_PC_Futures)
//	{
//		g_InstrumentManager.Add(pInstrument);
//		char msg[1000];
//		memset(msg,0,sizeof(msg));
//		sprintf(msg,"%s,instrument: %s,productID: %s\n",__FUNCTION__,pInstrument->InstrumentID,pInstrument->ProductID);
//		printf(msg);
//	}
//
//	if (bIsLast)
//	{
//	    printf("RspQryInstrument Feedback isLast\n");
//	    //AddInstrumentSpecs();
//        //m_Event.event_set();
//	}
//
};

///请求查询行情响应
void TradeHandler::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

  printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  //fn_printCout(__FUNCTION__);
};

///请求查询结算信息确认响应
void TradeHandler::OnRspQrySettlementInfoConfirm(
    CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, 
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

  //fn_printCout(__FUNCTION__);

  //if (pRspInfo->ErrorID != 0) {
  //  // Confirm Settlement Failed
  //  std::cout<<">>>>>>> Trade API Failure"<<std::endl;
  //  printf("Failed to confirm settlement, errorcode=%d errormsg=%s requestid=%d chain=%d", 
  //      pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
  //} else {
  //  // Confirm Settlement Failed
  //  std::cout<<">>>>>>> Trade API Confirmed Settlement"<<std::endl;
  //}
};

///错误应答
void TradeHandler::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  //fn_printCout(__FUNCTION__);
};

//std::string TradeHandler::extractInternalID(char *orderRef) {
//  std::string tmp(orderRef + ORDER_DIGIT);
//  return tmp;
//}

void TradeHandler::OnRtnTrade(CThostFtdcTradeField *pTrade) {
  //std::string chan = "CTPTradeReturn";

  //ReturnInfo rtnInfo;
  //rtnInfo.id = extractInternalID(pTrade->OrderRef);
  //rtnInfo.symbol = std::string(pTrade->InstrumentID);
  //rtnInfo.price = pTrade->Price;
  //rtnInfo.qty = pTrade->Volume; 

  //if (pTrade->Direction == THOST_FTDC_D_Buy) {
  //  rtnInfo.side = Direction::BUY;  
  //} else if (pTrade->Direction == THOST_FTDC_D_Sell) {
  //  rtnInfo.side = Direction::SELL;
  //}

  //rtnInfo.type = ReturnType::TRADE;
  //std::string msg = rtnInfo.serialize(); 
  //msgHub.send(chan, msg);

  //LOG(INFO) << "OnRtnOrder send to OrderHub msg " << msg << std::endl;
}

void TradeHandler::SendMsg(unsigned char type, char* pObj) {
  //CDX_INFO* new_info = new CDX_INFO();
  //CThostFtdcOrderField* pOrder = NULL;
  //CThostFtdcTradeField* pTrade = NULL;
  //OnRspOrderInsertMsg* pOnRspOrderInsertMsg = NULL;
  //pSharedCEM = g_ShareMem.m_pSharedObj;
  //char tempOrderRef[9];

  //int reqID = 0;
  //int exeOrderRef = 0;

  //switch (type) {
  //case TD_OnRtnOrder:
  //    pOrder = (CThostFtdcOrderField*)pObj;
  //    PrintOrder(pOrder);
  //    strcpy(tempOrderRef,pOrder->OrderRef+5);
  //    exeOrderRef = atoi(tempOrderRef);
  //    sprintf(pOrder->OrderRef,"%d",exeOrderRef);
  //    memcpy(&new_info->rtnOrder,pOrder,sizeof(new_info->rtnOrder));
  //    new_info->info_type = TD_OnRtnOrder;
  //    break;

  //case TD_OnRtnTrade:
  //    pTrade = (CThostFtdcTradeField*)pObj;
  //    PrintTrade(pTrade);
  //    strcpy(tempOrderRef,pTrade->OrderRef+5);
  //    exeOrderRef = atoi(tempOrderRef);
  //    sprintf(pTrade->OrderRef,"%d",exeOrderRef);
  //    memcpy(&new_info->rtnTrade,pTrade,sizeof(new_info->rtnTrade));
  //    new_info->info_type = TD_OnRtnTrade;
  //    break;

  //case TD_OnRspOrderInsert:
  //    pOnRspOrderInsertMsg = (OnRspOrderInsertMsg*)pObj;
  //    PrintOrderInsertErr(pOnRspOrderInsertMsg);
  //    strcpy(tempOrderRef,pOnRspOrderInsertMsg->InputOrder.OrderRef+5);
  //    exeOrderRef = atoi(tempOrderRef);
  //    sprintf(pOnRspOrderInsertMsg->InputOrder.OrderRef,"%d",exeOrderRef);
  //    memcpy(&new_info->rtnInputOrder,pOnRspOrderInsertMsg,sizeof(new_info->rtnInputOrder));
  //    new_info->info_type = TD_OnRspOrderInsert;
  //    break;

  //default:
  //    break;
  //}

  //reqID = exeOrderRef % 10000;
  //if(reqID > 0 && reqID < CDX_ACCOUNT_CNT && pSharedCEM->AcctExist(reqID))
  //{
  //    pSharedCEM->REGISTEREDACCT[reqID].INFO.push(*new_info);
  //    //printf("PUSH ALGO ORDER RTN to ACCT: %d\n",reqID);
  //}
  //else
  //{
  //    printf("MANUAL TRADE/ORDER, NOT BELONG TO ALGOS\n");
  //}

  //delete new_info;
}

void TradeHandler::PrintOrder(CThostFtdcOrderField* pOda) {
	//fprintf(stdout, "%s,%s,%s,%s,%d,%s,%d,%d,%d,%s,%s,%.3f,%d,%d,%d\n",
	//					pOda->InstrumentID,
	//					pOda->Direction == THOST_FTDC_D_Buy ? "BUY" : "SELL",
	//					pOda->TradingDay,
	//					pOda->InsertTime,
	//					pOda->RequestID,
	//					pOda->OrderRef,
	//					pOda->BrokerOrderSeq,
	//					pOda->SequenceNo,
	//					pOda->NotifySequence,
	//					CTPOrderStatToString(pOda->OrderStatus).c_str(),
	//					CTPOrderSubmitStatToString(pOda->OrderSubmitStatus).c_str(),
  //                      pOda->LimitPrice,
	//					pOda->VolumeTotalOriginal,
	//					pOda->VolumeTraded,
	//					pOda->VolumeTotal);
	//fflush(g_CDXLog.m_pOrderLogFile);

	//CDX_LOGINFO log;
	//log.LOGTYPE = 1;
	//log.INFOTYPE= INFO_LIVE;
	//memcpy(&log.rtnOrder,pOda,sizeof(log.rtnOrder));
	//g_public_chan->RSLT_QUEUE.push(log);
}

void TradeHandler::PrintTrade(CThostFtdcTradeField* pTda) {
	//fprintf(g_CDXLog.m_pTradeLogFile, "%s,%s,%d,%d,%s,%s,%s,%.3f,%d,%s,%s\n",
	//					pTda->OrderRef,
	//					pTda->TradeID,
	//					pTda->SequenceNo,
	//					pTda->BrokerOrderSeq,
	//					CTPHedgeFlagToString(pTda->OffsetFlag).c_str(),
	//					pTda->Direction == THOST_FTDC_D_Buy ? "BUY" : "SELL",
	//					pTda->InstrumentID,
	//					pTda->Price,
	//					pTda->Volume,
	//					pTda->TradeDate,
	//					pTda->TradeTime);
	//fflush(g_CDXLog.m_pTradeLogFile);
	//CDX_LOGINFO log;
	//log.LOGTYPE = 2;
	//log.INFOTYPE= INFO_LIVE;
	//memcpy(&log.rtnTrade,pTda,sizeof(log.rtnTrade));
	//g_public_chan->RSLT_QUEUE.push(log);
}

//void TradeHandler::PrintOrderInsertErr(OnRspOrderInsertMsg* pErr) {
  //char ErrTime[10] = {0};
  //time_t t =time(NULL);
	//struct tm *ti;
	//ti = localtime(&t);
	//int ErrDate = (ti->tm_year + 1900) * 10000 + (ti->tm_mon + 1) * 100 + ti->tm_mday;
  //  strftime(ErrTime,sizeof(ErrTime),"%H:%M:%S",ti);

  //  fprintf(g_CDXLog.m_pErrorLogFile,"%d,%s,%d,%s,%s,%d,%f,%d,%s\n",
  //                      pErr->InputOrder.RequestID,
  //                      pErr->InputOrder.OrderRef,
  //                      ErrDate,
  //                      ErrTime,
  //                      pErr->InputOrder.Direction == THOST_FTDC_D_Buy ? "BUY":"SELL",
  //                      pErr->InputOrder.VolumeTotalOriginal,
  //                      pErr->InputOrder.LimitPrice,
  //                      pErr->RspInfo.ErrorID,
  //                      pErr->RspInfo.ErrorMsg);
  //  fflush(g_CDXLog.m_pErrorLogFile);
	//CDX_LOGINFO log;
	//log.LOGTYPE = 3;
	//log.INFOTYPE= INFO_LIVE;
	//memcpy(&log.rtnInputOrder,pErr,sizeof(log.rtnInputOrder));
	//g_public_chan->RSLT_QUEUE.push(log);
//}
