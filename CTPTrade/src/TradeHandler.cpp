#include "TradeHandler.h"
#include "CedarHelper.h"
#include "LogHelper.h"

TradeHandler::TradeHandler() {
  pUserTradeApi = CThostFtdcTraderApi::CreateFtdcTraderApi();

  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(std::bind(&TradeHandler::onMsg,
      this, std::placeholders::_1));
}

int TradeHandler::start() {
  std::string tradeFront;
  CedarJsonConfig::getInstance().getStringByPath("CTP.TradeFront", tradeFront);
  LOG(INFO) << "connect to trade front" << tradeFront;
  pUserTradeApi->RegisterSpi(this);

  pUserTradeApi->SubscribePrivateTopic(THOST_TERT_QUICK);
  pUserTradeApi->SubscribePublicTopic(THOST_TERT_QUICK);

  pUserTradeApi->RegisterFront(const_cast<char*>(tradeFront.c_str()));
  pUserTradeApi->Init();

  return 0;
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

  pUserTradeApi->ReqUserLogin(&req, 0);
}

void TradeHandler::OnRspUserLogin(
    CThostFtdcRspUserLoginField *pRspUserLogin,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

  if (pRspInfo->ErrorID != 0) {
    // fail to login !!!
    LOG(ERROR) << "Failed to login !";
    LOG(ERROR) << "Login ErrorCode:" << pRspInfo->ErrorID
              << " ErrorMsg:" << pRspInfo->ErrorMsg
              << " RequestID:" << nRequestID
              << " Chain:" << bIsLast;
    exit(-1);
  }

  LOG(INFO) <<">>>> CTP Trade API: Login Successfully";

  //memcpy(&g_Login, pRspUserLogin, sizeof(CThostFtdcRspUserLoginField));
  frontID = pRspUserLogin->FrontID;
  sessionID = pRspUserLogin->SessionID;

  CThostFtdcSettlementInfoConfirmField pSettlementConfirm;
  strcpy(pSettlementConfirm.BrokerID, brokerId.c_str());
  strcpy(pSettlementConfirm.InvestorID, userId.c_str());
  pUserTradeApi->ReqSettlementInfoConfirm(&pSettlementConfirm,0);

  LOG(INFO) << "brokerId " << brokerId;
  LOG(INFO) << "userId " << userId;
}

int TradeHandler::onMsg(MessageBase msg) {
  if (msg.type() == TYPE_ORDER_REQUEST) {
    OrderRequest orderReq = ProtoBufHelper::unwrapMsg<OrderRequest>(msg);
    sendOrderReq(orderReq);
  } else {
    LOG(WARNING) << "recv invalid msg type " << msg.type();
  }

  return 0;
}

int TradeHandler::initReq(CThostFtdcInputOrderField &req) {
  req.TimeCondition = THOST_FTDC_TC_GFD;
  strcpy(req.GTDDate, "");

  //hedge flag
  req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
  //volume condition
  req.VolumeCondition = THOST_FTDC_VC_AV;
  //minimum volume : 1
  req.MinVolume = 1;
  req.ContingentCondition = THOST_FTDC_CC_Immediately;
  req.StopPrice = 0;
  //reason of forcible of close
  req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
  //is auto suspend
  req.IsAutoSuspend = 0;
  //is forcible close
  req.UserForceClose = 0;

  strcpy(req.BrokerID, brokerId.c_str());
  strcpy(req.InvestorID, userId.c_str());
  strcpy(req.UserID, userId.c_str());

  return 0;
}

int TradeHandler::sendOrderReq(OrderRequest &req) {

  if (req.type() == TYPE_LIMIT_ORDER_REQUEST ||
      req.type() == TYPE_MARKET_ORDER_REQUEST) {

    std::string reqId = getIncreaseID();

    CThostFtdcInputOrderField ctpReq;
    memset(&ctpReq, 0, sizeof(ctpReq));

    initReq(ctpReq);

    strcpy(ctpReq.InstrumentID, req.code().c_str());
    strcpy(ctpReq.OrderRef, reqId.c_str());

    ctpReq.RequestID = std::atoi(reqId.c_str());
    ctpReq.Direction =
      (req.buy_sell() == LONG_BUY ? THOST_FTDC_D_Buy : THOST_FTDC_D_Sell);

    ctpReq.VolumeTotalOriginal = req.trade_quantity();

    if (req.open_close() == OPEN_POSITION)
      ctpReq.CombOffsetFlag[0]  = THOST_FTDC_OF_Open;
    else if (req.open_close() == CLOSE_TODAY_POSITION)
      ctpReq.CombOffsetFlag[0] = THOST_FTDC_OF_CloseToday;
    else if (req.open_close() == CLOSE_POSITION)
      ctpReq.CombOffsetFlag[0] = THOST_FTDC_OF_Close;
    else if (req.open_close() == CLOSE_YESTERDAY_POSITION)
      ctpReq.CombOffsetFlag[0] = THOST_FTDC_OF_CloseYesterday;
    else {
      LOG(ERROR) << "Recv Invalid open_close in new order request";
      //need to send response
    }

    if (req.type() == TYPE_LIMIT_ORDER_REQUEST) {
      ctpReq.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
      ctpReq.LimitPrice = req.limit_price();
    } else if (req.type() == TYPE_MARKET_ORDER_REQUEST) {
      ctpReq.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
      ctpReq.LimitPrice = 0;
    }

    pUserTradeApi->ReqOrderInsert(&ctpReq, 1);

    //record in idmap
    std::string inId = req.id();
    inIdToExId[req.id()] = reqId;
    exIdToInId[reqId] = req.id();

    inToCTPReq[req.id()] = {
      req.id(), req.response_address(), req.type(), req.cancel_order_id(),
      req.trade_quantity(), req.trade_quantity() };

    LOG(INFO) << "request ID " << ctpReq.RequestID << std::endl;
    LOG(INFO) << "recv new order req inID " << req.id() << " exID " << reqId;
    LOG(INFO) << "user ctp request " << req.DebugString();

  } else if (req.type() == TYPE_CANCEL_ORDER_REQUEST) {

    CThostFtdcInputOrderActionField cnclReq;
    memset(&cnclReq, 0, sizeof(cnclReq));

    std::string toCancelInId = req.cancel_order_id();
    if (inIdToExId.find(toCancelInId) == inIdToExId.end()) {
      LOG(ERROR) << "Can't find exId " << toCancelInId << " for canceling !";
      return -1;
    }

    if (inToCTPReq.find(toCancelInId) == inToCTPReq.end()) {
      LOG(ERROR) << "Can't find inId for object to get orderSysId!";
      return -1;
    }
    strcpy(cnclReq.ExchangeID, inToCTPReq[toCancelInId].exchangeId);
    strcpy(cnclReq.OrderSysID, inToCTPReq[toCancelInId].orderSysId);

    cnclReq.ActionFlag = THOST_FTDC_AF_Delete;
    cnclReq.FrontID = frontID;
    cnclReq.SessionID = sessionID;

    strcpy(cnclReq.BrokerID, brokerId.c_str());
    strcpy(cnclReq.UserID, userId.c_str());
    strcpy(cnclReq.InvestorID, userId.c_str());
    strcpy(cnclReq.InstrumentID, req.code().c_str());

    pUserTradeApi->ReqOrderAction(&cnclReq, 2);

    LOG(INFO) << "send CTP request orderRef:" << cnclReq.OrderRef
      << " actionOrdRef:" << cnclReq.OrderActionRef;
    LOG(INFO) << "user ctp cancel request " << req.DebugString();
  } else {
    LOG(ERROR) << "recv an invalid order req type" << req.type();
    return -1;
  }

  LogHelper::logObject(req);
  return 0;
}

void TradeHandler::OnRspOrderInsert(
  CThostFtdcInputOrderField *pInputOrder,
  CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

  LOG(INFO) << __FUNCTION__ << std::endl;

  sendErrorResponse(pInputOrder, pRspInfo);
}

int TradeHandler::recycleID(std::string &exId, std::string &inId) {
  LOG(INFO) << "recycle all exId:" << exId << ", inId:" << inId << std::endl;

  if (exIdToInId.find(exId) == exIdToInId.end()) {
    LOG(WARNING) << "Can't find exId [" << exId << "] and mapping inId";
  } else {
    exIdToInId.erase(exId);
  }

  if (inToCTPReq.find(inId) == inToCTPReq.end()) {
    LOG(WARNING) << "Can't find inId " << inId << " and mapping OrderRequest";
  } else {
    inToCTPReq.erase(inId);
  }

  if (inIdToExId.find(inId) == inIdToExId.end()) {
    LOG(WARNING) << "Can't find inId " << inId << " and mapping ExId";
  } else {
    inIdToExId.erase(inId);
  }

  return 0;
}

std::string orderStatusToString(TThostFtdcOrderStatusType status) {
  switch (status) {
    case '0':
      return "AllTraded";
    case '1':
      return "PartTradedQueueing";
    case '2':
      return "PartTradedNotQueueing";
    case '3':
      return "NoTradeQueueing";
    case '4':
      return "NoTradeNotQueueing";
    case '5':
      return "Cancel";
    case 'a':
      return "Unknown";
    case 'b':
      return "NotTouched";
    case 'c':
      return "Touched";
  }
  return "Error";
}

std::string submitStatusTypeToString(TThostFtdcOrderSubmitStatusType status) {
  switch (status) {
    case '0':
      return "InsertSubmitted";
    case '1':
      return "CancelSubmitted";
    case '2':
      return "ModifySubmitted";
    case '3':
      return "Accepted";
    case '4':
      return "InsertRejected";
    case '5':
      return "CancelRejected";
    case '6':
      return "ModifyRejected";
  }
  return "Error";
}

void TradeHandler::OnRtnOrder(CThostFtdcOrderField *pOrder) {
  LOG(INFO) << __FUNCTION__ << std::endl;
  LOG(INFO) << std::endl <<
    "OrderRef: " <<  pOrder->OrderRef << std::endl <<
    ",Direction:" << (pOrder->Direction == THOST_FTDC_D_Buy ? "BUY":"SELL") <<
    ",OrderStatus:" << orderStatusToString(pOrder->OrderStatus) <<
    ",SubmitStatus:"<<
      submitStatusTypeToString(pOrder->OrderSubmitStatus) << std::endl <<
    "InvestorID:" << pOrder->InvestorID << std::endl <<
    "Code:" << pOrder->InstrumentID << std::endl <<
    "UserID:" << pOrder->UserID << std::endl <<
    "RequestID:" << pOrder->RequestID << std::endl;

  if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted &&
    pOrder->OrderStatus == THOST_FTDC_OST_NoTradeQueueing) {

    ResponseMessage rsp;
    std::string exId = pOrder->OrderRef;

    rsp.set_type(TYPE_NEW_ORDER_CONFIRM);
    rsp.set_ref_id(exId);

    if (exIdToInId.find(exId) == exIdToInId.end()) {
      LOG(WARNING) << "Can't find exId" << exId << " and inId";
      return;
    }
    std::string inId = exIdToInId[exId];

    if (inToCTPReq.find(inId) == inToCTPReq.end()) {
      LOG(WARNING) << "Can't find inId" << exId << " and mapping OrderRequest";
      return;
    }

    strcpy(inToCTPReq[inId].exchangeId, pOrder->ExchangeID);
    strcpy(inToCTPReq[inId].orderSysId, pOrder->OrderSysID);

    msgHub.pushMsg(inToCTPReq[inId].responseAddr,
        ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rsp));

    LOG(INFO) << "send to addr " << inToCTPReq[inId].responseAddr;
    LOG(INFO) << "send NewOrderConfirm " << rsp.DebugString();
    LogHelper::logObject(rsp);

    return;
  } else if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled &&
      pOrder->OrderSubmitStatus == THOST_FTDC_OSS_Accepted) {

    ResponseMessage rsp;
    std::string exId = pOrder->OrderRef;
    if (exIdToInId.find(exId) == exIdToInId.end()) {
      LOG(WARNING) << "Can't find exId" << exId << " and inId";
      return;
    }

    std::string inId = exIdToInId[exId];
    if (inToCTPReq.find(inId) == inToCTPReq.end()) {
      LOG(WARNING) << "Can't find inId" << exId << " and mapping OrderRequest";
      return;
    }

    rsp.set_type(TYPE_CANCEL_ORDER_CONFIRM);
    rsp.set_id(getIncreaseID());
    rsp.set_ref_id(inToCTPReq[inId].cancel_id);

    msgHub.pushMsg(inToCTPReq[inId].responseAddr,
        ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rsp));

    LOG(INFO) << "send CancelConfirm" << rsp.DebugString();
    LogHelper::logObject(rsp);
    return;

  } else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_InsertRejected) {
    ResponseMessage rsp;
    std::string exId = pOrder->OrderRef;
    if (exIdToInId.find(exId) == exIdToInId.end()) {
      LOG(WARNING) << "Can't find exId" << exId << " and inId";
      return;
    }
    std::string inId = exIdToInId[exId];
    if (inToCTPReq.find(inId) == inToCTPReq.end()) {
      LOG(WARNING) << "Can't find inId" << exId << " and mapping OrderRequest";
      return;
    }

    rsp.set_type(TYPE_ERROR);
    rsp.set_id(getIncreaseID());
    rsp.set_ref_id(inId);
    rsp.set_error_msg("CTP order insert reject");

    msgHub.pushMsg(inToCTPReq[inId].responseAddr,
        ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rsp));
    recycleID(exId, inId);
    LogHelper::logObject(rsp);

  } else if (pOrder->OrderSubmitStatus == THOST_FTDC_OSS_CancelRejected) {
    ResponseMessage rsp;
    rsp.set_type(TYPE_ERROR);
    rsp.ref_id();
  }
}

int TradeHandler::close() {
  //release
  return 0;
}

void TradeHandler::OnFrontDisconnected(int nReason) {
  LOG(WARNING) << "Front Disconnected, quit "<<std::endl;
  exit(-1);
}

int TradeHandler::returnErrorInfo(CThostFtdcRspInfoField *pRspInfo) {
  LOG(ERROR) << __FUNCTION__ << "ErrorCode: " << pRspInfo->ErrorID
            << " ErrorMsg:" << pRspInfo->ErrorMsg;
  return 0;
}

int TradeHandler::sendErrorResponse(CThostFtdcInputOrderField *pInputOrder,
  CThostFtdcRspInfoField *pRspInfo) {

  std::string exId = pInputOrder->OrderRef;
  if (exIdToInId.find(exId) == exIdToInId.end()) {
    LOG(WARNING) << "Can't find exId " << exId << " and mapping inId";
    return -1;
  }

  std::string inId = exIdToInId[exId];
  if (inToCTPReq.find(inId) == inToCTPReq.end()) {
    LOG(WARNING) << "Can't find inId" << exId << " and mapping OrderRequest";
    return -1;
  }

  CTPUserRequest req = inToCTPReq[inId];
  ResponseMessage rsp;
  rsp.set_type(TYPE_ERROR);
  rsp.set_ref_id(inId);
  rsp.set_error_code(pRspInfo->ErrorID);
  rsp.set_error_msg("Check error msg in log file");

  LOG(INFO) << rsp.DebugString();

  msgHub.pushMsg(req.responseAddr,
      ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rsp));
  recycleID(exId, inId);

  LogHelper::logObject(rsp);

  return 0;
}

void TradeHandler::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder,
  CThostFtdcRspInfoField *pRspInfo) {

  LOG(INFO) << __FUNCTION__ << std::endl;

  sendErrorResponse(pInputOrder, pRspInfo);
  return;
}

void TradeHandler::OnErrRtnOrderAction(
  CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo) {
  LOG(INFO) << __FUNCTION__ << std::endl;

  //returnErrorInfo(pRspInfo);

  //printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID,
  //pRspInfo->ErrorMsg);
  ////fprintf(f_errorLog,"%d ErrorCode=[%d], ErrorMsg=[%s]\n", dataLength,
  //pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  //fflush(f_errorLog);
  //fn_printCout(__FUNCTION__);
}

void TradeHandler::OnRspParkedOrderInsert(
    CThostFtdcParkedOrderField *pParkedOrder,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

  //printf("ErrorCode=[%d], ErrorMsg=[%s]\n",
  //pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  //fn_printCout(__FUNCTION__);
}

//onRspfor cancel an order
void TradeHandler::OnRspOrderAction(
    CThostFtdcInputOrderActionField *pInputOrderAction,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

  printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  //fprintf(f_errorLog,"%d ErrorCode=[%d], ErrorMsg=[%s]\n", dataLength, pRspInfo->ErrorID, pRspInfo->ErrorMsg);
  //fflush(f_errorLog);
  //fn_printCout(__FUNCTION__);
}

///请求查询报单响应
void TradeHandler::OnRspQryOrder(CThostFtdcOrderField *pOrder,
  CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

  printf("ErrorCode=[%d], ErrorMsg=[%s]\n",
        pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    //fprintf(f_errorLog,"%d ErrorCode=[%d], ErrorMsg=[%s]\n", dataLength, pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    //fflush(f_errorLog);
    //fn_printCout(__FUNCTION__);
}

///请求查询成交响应
void TradeHandler::OnRspQryTrade(CThostFtdcTradeField *pTrade,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    LOG(ERROR) << "ErrorCode=[" << pRspInfo->ErrorID << "], ErrorMsg=["
      << pRspInfo->ErrorMsg << "]";
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
void TradeHandler::OnRspQryDepthMarketData(
    CThostFtdcDepthMarketDataField *pDepthMarketData,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  LOG(ERROR) << "ErrorCode=[" << pRspInfo->ErrorID << "], ErrorMsg=["
    << pRspInfo->ErrorMsg << "]";
};

///请求查询结算信息确认响应
void TradeHandler::OnRspQrySettlementInfoConfirm(
    CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

  //fn_printCout(__FUNCTION__);

  //if (pRspInfo->ErrorID != 0) {
  //  // Confirm Settlement Failed
  //  std::cout<<">>>>>>> Trade API Failure"<<std::endl;
  //  printf("Failed to confirm settlement, errorcode=%d errormsg=%s
  //  requestid=%d chain=%d",
  //      pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
  //} else {
  //  // Confirm Settlement Failed
  //  std::cout<<">>>>>>> Trade API Confirmed Settlement"<<std::endl;
  //}
};

///错误应答
void TradeHandler::OnRspError(
  CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  LOG(ERROR) << "ErrorCode=[" << pRspInfo->ErrorID << "], ErrorMsg=["
    << pRspInfo->ErrorMsg << "]";
};

void TradeHandler::OnRtnTrade(CThostFtdcTradeField *pTrade) {
  LOG(INFO) << __FUNCTION__;

  ResponseMessage rsp;

  std::string exId = pTrade->OrderRef;
  if (exIdToInId.find(exId) == exIdToInId.end()) {
    LOG(WARNING) << "Can't find exId" << exId << " and inId";
    return;
  }
  std::string inId = exIdToInId[exId];

  if (inToCTPReq.find(inId) == inToCTPReq.end()) {
    LOG(WARNING) << "Can't find inId" << exId << " and mapping OrderRequest";
    return;
  }

  rsp.set_type(TYPE_TRADE);
  rsp.set_code(pTrade->InstrumentID);
  rsp.set_price(pTrade->Price);
  rsp.set_trade_quantity(pTrade->Volume);

  if (pTrade->Direction == THOST_FTDC_D_Buy) {
    rsp.set_buy_sell(TradeDirection::LONG_BUY);
  } else if (pTrade->Direction == THOST_FTDC_D_Sell) {
    rsp.set_buy_sell(TradeDirection::SHORT_SELL);
  }

  msgHub.pushMsg(inToCTPReq[inId].responseAddr,
      ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rsp));
  LOG(INFO) << "OnRtnTrade send trade " << rsp.DebugString() << std::endl;
  LogHelper::logObject(rsp);

  inToCTPReq[inId].leftQty -= pTrade->Volume;
  if (inToCTPReq[inId].leftQty == 0) {
    recycleID(exId, inId);
  }
}
