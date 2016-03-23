#include "CatsMktUpdt.h"

using namespace std;

CatsMktUpdt::CatsMktUpdt() : csm(CatsSessionManager("CatsMktUpdt")) {
	LOG(INFO) << "[CatsMktUpdt init] setup MsgHub done.";
	//setup MsgHub
	ProtoBufHelper::setupProtoBufMsgHub(msgHub);
	msgHub.registerCallback(std::bind(&CatsMktUpdt::onMsg, this, std::placeholders::_1));
	LOG(INFO) << "[CatsMktUpdt init] setup MsgHub done.";
}
//save one id in heap space on every call
//caution on overflow
const char* CatsMktUpdt::heapSpaceStore(string id) {
	const int LENGTH = 1000;
	static string heapSpace[LENGTH];
	static int idx = 0;
	int tmp = idx++;
	idx %= LENGTH;
	heapSpace[tmp] = id;
	const char *pStr = (heapSpace[tmp]).c_str();
	return pStr;
}

//subscribe market data update
//update will be published if subscribe successfully
//increment publish, empty if market data remain the same
int CatsMktUpdt::subSingleSymbol(std::string sSymbol) {
	CATSAPI_PreSub_MarketData(csm.g_hHandle);
	csm.SetCatsAcct(csm.g_hHandle);
	CATSAPI_SetParam(csm.g_hHandle, "symbol", sSymbol.c_str());
	const char * symbol = heapSpaceStore(sSymbol);

	//make tuple with this pointer and symbol string
	auto* pTuple = new tuple<CatsMktUpdt*, const char*>(make_tuple(this, const_cast<char*>(symbol)));
	if (0 != CATSAPI_Subscribe_MarketData(csm.g_hHandle, PubMarketDataCallback, this, BizSubCommonCallback, pTuple)) {
		char errMsg[1024] = { 0 };
		CATSAPI_GetLastError(csm.g_hHandle, errMsg, sizeof(errMsg));
		LOG(ERROR) << "[subSingleSymbol] subscribing " << sSymbol << " market data update failed:" << errMsg;
		return -1;
	}
	
	return 0;
}

void CatsMktUpdt::BizSubCommonCallback(void* pArg) {
	CatsMktUpdt* pMktUpdt = get<0>(*static_cast<tuple<CatsMktUpdt*, const char*>*>(pArg));
	const char* symbol = get<1>(*static_cast<tuple<CatsMktUpdt*, const char*>*>(pArg));
	int nErrorCode = 0;
	char errMsg[1024] = { 0 };
	nErrorCode = CATSAPI_GetLastError(pMktUpdt->csm.g_hHandle, errMsg, sizeof(errMsg));

	if (nErrorCode != 0) {
		LOG(ERROR) << "[bizSubCommonCallback] MktUpdt Biz Sub failed! ErrorMsg: " << errMsg;
	}

	delete pArg;
	LOG(WARNING) << "[bizSubCommonCallback] sub Market data: " << symbol << " succeeded !";
}

#define INT_TO_DOUBLE(x)  (static_cast<double>(x) / 10000)
void CatsMktUpdt::PubMarketDataCallback(void* pArg) {
	CatsMktUpdt* pMktUpdt = static_cast<CatsMktUpdt*>(pArg);
	CATSAPI_Fetch_MarketDataResult(pMktUpdt->csm.g_hHandle);

	while (CATSAPI_GetNext(pMktUpdt->csm.g_hHandle) == 0) {
		int nBufLen = 0;
		char *pBuf = CATSAPI_GetBuffer(pMktUpdt->csm.g_hHandle, &nBufLen);
	
		if (nBufLen <= 0)
			return;

		//increment publish, empty if market data remain the same
		STRealMarketData_Depth *pRealMarketData = (STRealMarketData_Depth *)pBuf;

		MarketUpdate mktUpdt;
		//symbol is like 600000.SH use '.' to split
		std::string symb = pRealMarketData->symbol;
		mktUpdt.set_code(symb.substr(0, 6));
		mktUpdt.set_exchange(symb.substr(7, 2));

		mktUpdt.add_bid_price(INT_TO_DOUBLE(pRealMarketData->bidPrice1));
		mktUpdt.add_bid_price(INT_TO_DOUBLE(pRealMarketData->bidPrice2));
		mktUpdt.add_bid_price(INT_TO_DOUBLE(pRealMarketData->bidPrice3));
		mktUpdt.add_bid_price(INT_TO_DOUBLE(pRealMarketData->bidPrice4));
		mktUpdt.add_bid_price(INT_TO_DOUBLE(pRealMarketData->bidPrice5));

		mktUpdt.add_bid_volume(pRealMarketData->bidVol1);
		mktUpdt.add_bid_volume(pRealMarketData->bidVol2);
		mktUpdt.add_bid_volume(pRealMarketData->bidVol3);
		mktUpdt.add_bid_volume(pRealMarketData->bidVol4);
		mktUpdt.add_bid_volume(pRealMarketData->bidVol5);

		mktUpdt.add_ask_price(INT_TO_DOUBLE(pRealMarketData->askPrice1));
		mktUpdt.add_ask_price(INT_TO_DOUBLE(pRealMarketData->askPrice2));
		mktUpdt.add_ask_price(INT_TO_DOUBLE(pRealMarketData->askPrice3));
		mktUpdt.add_ask_price(INT_TO_DOUBLE(pRealMarketData->askPrice4));
		mktUpdt.add_ask_price(INT_TO_DOUBLE(pRealMarketData->askPrice5));

		mktUpdt.add_ask_volume(pRealMarketData->askVol1);
		mktUpdt.add_ask_volume(pRealMarketData->askVol2);
		mktUpdt.add_ask_volume(pRealMarketData->askVol3);
		mktUpdt.add_ask_volume(pRealMarketData->askVol4);
		mktUpdt.add_ask_volume(pRealMarketData->askVol5);

		mktUpdt.set_open_price(INT_TO_DOUBLE(pRealMarketData->openPrice));
		mktUpdt.set_last_price(INT_TO_DOUBLE(pRealMarketData->lastPrice));
		mktUpdt.set_highest_price(INT_TO_DOUBLE(pRealMarketData->highPrice));

		mktUpdt.set_lowest_price(INT_TO_DOUBLE(pRealMarketData->lowPrice));
		mktUpdt.set_high_limit_price(INT_TO_DOUBLE(pRealMarketData->highLimited));
		mktUpdt.set_low_limit_price(INT_TO_DOUBLE(pRealMarketData->lowLimited));
		mktUpdt.set_open_interest(pRealMarketData->openInterest);
		
		mktUpdt.set_turnover(INT_TO_DOUBLE(pRealMarketData->turnOver));
		mktUpdt.set_volume(pRealMarketData->volume);
		mktUpdt.set_exchange_timestamp(std::to_string(pRealMarketData->time));
		mktUpdt.set_recv_timestamp(CedarHelper::getCurTimeStamp());

		std::string res = ProtoBufHelper::wrapMsg<MarketUpdate>(TYPE_MARKETUPDATE, mktUpdt);
		pMktUpdt->msgHub.boardcastMsg(symb, res);
	}
}

//MsgHub callback fucntion on every msg
int CatsMktUpdt::onMsg(MessageBase msg) {
	if (msg.type() == TYPE_DATAREQUEST) {
		DataRequest dataReq = ProtoBufHelper::unwrapMsg<DataRequest>(msg);
		std::string chan = dataReq.code() + "." + dataReq.exchange();

		LOG(INFO) << "subscribe " << chan;
		subSingleSymbol(chan);

	} else {
		LOG(WARNING) << "Recv invalid msg type " << msg.type();
	}

	return 0;
}

