#include "stdafx.h"
#include "TdxApi.h"


CTdxApi::CTdxApi(void)
{
	this->m_TdxApiHMODULE = LoadLibrary("trade.dll");

	this->OpenTdx = (OpenTdxDelegate)GetProcAddress(this->m_TdxApiHMODULE, "OpenTdx");
	this->CloseTdx = (CloseTdxDelegate)GetProcAddress(this->m_TdxApiHMODULE, "CloseTdx");
	this->Logon = (LogonDelegate)GetProcAddress(this->m_TdxApiHMODULE, "Logon");
	this->Logoff = (LogoffDelegate)GetProcAddress(this->m_TdxApiHMODULE, "Logoff");
	this->QueryData = (QueryDataDelegate)GetProcAddress(this->m_TdxApiHMODULE, "QueryData");
	this->QueryDatas = (QueryDatasDelegate)GetProcAddress(this->m_TdxApiHMODULE, "QueryDatas");
	this->QueryHistoryData = (QueryHistoryDataDelegate)GetProcAddress(this->m_TdxApiHMODULE, "QueryHistoryData");
	this->SendOrder = (SendOrderDelegate)GetProcAddress(this->m_TdxApiHMODULE, "SendOrder");
	this->SendOrders = (SendOrdersDelegate)GetProcAddress(this->m_TdxApiHMODULE, "SendOrders");
	this->CancelOrder = (CancelOrderDelegate)GetProcAddress(this->m_TdxApiHMODULE, "CancelOrder");
	this->CancelOrders = (CancelOrdersDelegate)GetProcAddress(this->m_TdxApiHMODULE, "CancelOrders");
	this->GetQuote = (GetQuoteDelegate)GetProcAddress(this->m_TdxApiHMODULE, "GetQuote");
	this->GetQuotes = (GetQuotesDelegate)GetProcAddress(this->m_TdxApiHMODULE, "GetQuotes");
	this->Repay = (RepayDelegate)GetProcAddress(this->m_TdxApiHMODULE, "Repay");



	this->QueryHKData = (QueryHKDataDelegate)GetProcAddress(this->m_TdxApiHMODULE, "QueryHKData");
	this->QueryHKHistoryData = (QueryHKHistoryDataDelegate)GetProcAddress(this->m_TdxApiHMODULE, "QueryHKHistoryData");
	this->GetHKStockTradeInfo = (GetHKStockTradeInfoDelegate)GetProcAddress(this->m_TdxApiHMODULE, "GetHKStockTradeInfo");
	this->SendHKOrder = (SendHKOrderDelegate)GetProcAddress(this->m_TdxApiHMODULE, "SendHKOrder");
	this->CancelHKOrder = (CancelHKOrderDelegate)GetProcAddress(this->m_TdxApiHMODULE, "CancelHKOrder");
}


CTdxApi::~CTdxApi(void)
{
	//ÊÍ·ÅTdxApi.dllºÍ¿Õ¼ä
	FreeLibrary(this->m_TdxApiHMODULE);
}
