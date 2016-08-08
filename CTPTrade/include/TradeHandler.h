#ifndef TRADE_HANDLER_H
#define TRADE_HANDLER_H

#include <map>

#include "ProtoBufMsgHub.h"

//#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcUserApiStruct.h"

struct CTPUserRequest {
  std::string id;
  std::string responseAddr;
  RequestType type;
  std::string cancel_id;
  int originalQty;
  int leftQty;
  TThostFtdcExchangeIDType exchangeId;
  TThostFtdcOrderSysIDType	orderSysId;
};

class TradeHandler : public CThostFtdcTraderSpi {

public:
  TradeHandler();

  int start();
  int close();

private:
  int onMsg(MessageBase);

  inline std::string getIncreaseID() {
    int static id = 0;
    return std::to_string(++id);
  }

  //======= CTP API below ==========
  void OnFrontConnected();
  void OnFrontDisconnected(int nReason);
  void OnRspUserLogin(CThostFtdcRspUserLoginField *,CThostFtdcRspInfoField *,
      int nRequestID, bool bIsLast);

  //报单录入请求响应
  virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  //预埋单录入请求响应
  virtual void OnRspParkedOrderInsert(
    CThostFtdcParkedOrderField *pParkedOrder,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

  //报单操作请求响应
  virtual void OnRspOrderAction(
    CThostFtdcInputOrderActionField *pInputOrderAction,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	//请求查询报单响应
	virtual void OnRspQryOrder(CThostFtdcOrderField *pOrder,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询成交响应
	virtual void OnRspQryTrade(CThostFtdcTradeField *pTrade,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询合约响应
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询行情响应
	virtual void OnRspQryDepthMarketData(
      CThostFtdcDepthMarketDataField *pDepthMarketData,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///请求查询结算信息确认响应
	virtual void OnRspQrySettlementInfoConfirm(
      CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo,
      int nRequestID, bool bIsLast);

	///报单通知
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);

	///成交通知
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	///报单录入错误回报
	virtual void OnErrRtnOrderInsert(
    CThostFtdcInputOrderField *pInputOrder,
    CThostFtdcRspInfoField *pRspInfo);

	///报单操作错误回报
	virtual void OnErrRtnOrderAction(
    CThostFtdcOrderActionField *pOrderAction,
    CThostFtdcRspInfoField *pRspInfo);

	///合约交易状态通知
	virtual void OnRtnInstrumentStatus(
      CThostFtdcInstrumentStatusField *pInstrumentStatus) {
		//fn_printCout(__FUNCTION__);
//		/// CLOSE PROGRAM IF MARKET CLOSED
//		if(strcmp(pInstrumentStatus->InstrumentID,g_strInstrId) == 0)
//        {
//            if(pInstrumentStatus->InstrumentStatus == THOST_FTDC_IS_Closed)
//            {
//                if(pInstrumentStatus->EnterTime > "14:59:00")
//                {
//                    fn_eodProcess();
//                }
//
//            }
//        }
	};

	///交易通知
	virtual void OnRtnTradingNotice(CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo) {
		//fn_printCout(__FUNCTION__);
	}

	///提示条件单校验错误
	virtual void OnRtnErrorConditionalOrder(
      CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder) {
		//fn_printCout(__FUNCTION__);
	};

  ///心跳超时警告。当长时间未收到报文时，该方法被调用。
  ///@param nTimeLapse 距离上次接收报文的时间
  void OnHeartBeatWarning(int nTimeLapse) {
    //fn_printCout(__FUNCTION__);
  }

  ///登出请求响应
  void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    //fn_printCout(__FUNCTION__);
  }

	///请求查询预埋单响应
	virtual void OnRspQryParkedOrder(CThostFtdcParkedOrderField *pParkedOrder,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		//fn_printCout(__FUNCTION__);
	};

	///请求查询预埋撤单响应
	virtual void OnRspQryParkedOrderAction(
      CThostFtdcParkedOrderActionField *pParkedOrderAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		//fn_printCout(__FUNCTION__);
	};

	///请求查询交易通知响应
	virtual void OnRspQryTradingNotice(
      CThostFtdcTradingNoticeField *pTradingNotice,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		//fn_printCout(__FUNCTION__);
	};

	///请求查询经纪公司交易参数响应
	virtual void OnRspQryBrokerTradingParams(
      CThostFtdcBrokerTradingParamsField *pBrokerTradingParams,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		//fn_printCout(__FUNCTION__);
	};

	///请求查询经纪公司交易算法响应
	virtual void OnRspQryBrokerTradingAlgos(
      CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
		//fn_printCout(__FUNCTION__);
	};

  //预埋撤单录入请求响应
  void OnRspParkedOrderAction(
      CThostFtdcParkedOrderActionField *pParkedOrderAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    //fn_printCout(__FUNCTION__);
  };

  ///请求查询投资者结算结果响应
  void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

    //fn_printCout(__FUNCTION__);
  };

  ///请求查询转帐银行响应
  void OnRspQryTransferBank(CThostFtdcTransferBankField *pTransferBank,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

    //fn_printCout(__FUNCTION__);
  };

  ///请求查询投资者持仓明细响应
  void OnRspQryInvestorPositionDetail(
    CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

    //fn_printCout(__FUNCTION__);
  };

  ///请求查询客户通知响应
  void OnRspQryNotice(CThostFtdcNoticeField *pNotice,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

    //fn_printCout(__FUNCTION__);
  };

  ///请求查询投资者持仓响应
  void OnRspQryInvestorPosition(
    CThostFtdcInvestorPositionField *pInvestorPosition,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

    //fn_printCout(__FUNCTION__);
  };

  ///请求查询资金账户响应
  void OnRspQryTradingAccount(
    CThostFtdcTradingAccountField *pTradingAccount,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    //fn_printCout(__FUNCTION__);
  };

  ///请求查询投资者响应
  void OnRspQryInvestor(
    CThostFtdcInvestorField *pInvestor,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

    //fn_printCout(__FUNCTION__);
  };

  ///请求查询交易编码响应
  void OnRspQryTradingCode(CThostFtdcTradingCodeField *pTradingCode,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

    //fn_printCout(__FUNCTION__);
  };

  ///请求查询合约保证金率响应
  void OnRspQryInstrumentMarginRate(
    CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

    //fn_printCout(__FUNCTION__);
  };

  ///请求查询合约手续费率响应
  void OnRspQryInstrumentCommissionRate(
    CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

    //fn_printCout(__FUNCTION__);
  };

  ///请求查询交易所响应
  void OnRspQryExchange(
    CThostFtdcExchangeField *pExchange,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    //fn_printCout(__FUNCTION__);
  };

  //查询最大报单数量响应
  void OnRspQueryMaxOrderVolume(
    CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    //fn_printCout(__FUNCTION__);
  };

  //投资者结算结果确认响应
  void OnRspSettlementInfoConfirm(
    CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

    //fn_printCout(__FUNCTION__);
  };

  //删除预埋单响应
  void OnRspRemoveParkedOrder(
    CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {

    //fn_printCout(__FUNCTION__);
  };

  //删除预埋撤单响应
  void OnRspRemoveParkedOrderAction(
    CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    //fn_printCout(__FUNCTION__);
  };

  ///请求查询投资者持仓明细响应
  void OnRspQryInvestorPositionCombineDetail(
  CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail,
  CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    //fn_printCout(__FUNCTION__);
  };

  CThostFtdcTraderApi *pUserTradeApi;
  ProtoBufMsgHub msgHub;

  std::string brokerId;
  std::string password;
  std::string userId;
  TThostFtdcFrontIDType frontID;
  TThostFtdcSessionIDType	sessionID;

  std::map<std::string, std::string> inIdToExId;
  std::map<std::string, std::string> exIdToInId;
  std::map<std::string, CTPUserRequest> inToCTPReq;

  int initReq(CThostFtdcInputOrderField&);
  int sendOrderReq(OrderRequest&);
  int returnErrorInfo(CThostFtdcRspInfoField *);
  int recycleID(std::string&, std::string&);
  int sendErrorResponse(CThostFtdcInputOrderField*, CThostFtdcRspInfoField*);
};

#endif
