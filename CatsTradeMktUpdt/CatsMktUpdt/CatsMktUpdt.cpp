#include "CatsMktUpdt.h"

using namespace std;

CatsMktUpdt::CatsMktUpdt() : csm(CatsSessionManager("CatsMktUpdt")) {
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
	LOG(WARNING) << "[bizSubCommonCallback] sub Market data: " << symbol << " succeeded";
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
		//MarketUpdate mrk;
		//mrk.security = pRealMarketData->symbol;
		//mrk.askPrice.push_back(static_cast<double>(pRealMarketData->askPrice1) / 10000);
		//mrk.askPrice.push_back(static_cast<double>(pRealMarketData->askPrice2) / 10000);
		//mrk.askPrice.push_back(static_cast<double>(pRealMarketData->askPrice3) / 10000);
		//mrk.askPrice.push_back(static_cast<double>(pRealMarketData->askPrice4) / 10000);
		//mrk.askPrice.push_back(static_cast<double>(pRealMarketData->askPrice5) / 10000);
		//mrk.bidPrice.push_back(static_cast<double>(pRealMarketData->bidPrice1) / 10000);
		//mrk.bidPrice.push_back(static_cast<double>(pRealMarketData->bidPrice2) / 10000);
		//mrk.bidPrice.push_back(static_cast<double>(pRealMarketData->bidPrice3) / 10000);
		//mrk.bidPrice.push_back(static_cast<double>(pRealMarketData->bidPrice4) / 10000);
		//mrk.bidPrice.push_back(static_cast<double>(pRealMarketData->bidPrice5) / 10000);
		//mrk.askVolume.push_back(pRealMarketData->askVol1);
		//mrk.askVolume.push_back(pRealMarketData->askVol2);
		//mrk.askVolume.push_back(pRealMarketData->askVol3);
		//mrk.askVolume.push_back(pRealMarketData->askVol4);
		//mrk.askVolume.push_back(pRealMarketData->askVol5);
		//mrk.bidVolume.push_back(pRealMarketData->bidVol1);
		//mrk.bidVolume.push_back(pRealMarketData->bidVol2);
		//mrk.bidVolume.push_back(pRealMarketData->bidVol3);
		//mrk.bidVolume.push_back(pRealMarketData->bidVol4);
		//mrk.bidVolume.push_back(pRealMarketData->bidVol5);

		//mrk.openPrice = INT_TO_DOUBLE(pRealMarketData->openPrice);
		//mrk.lastPrice = INT_TO_DOUBLE(pRealMarketData->lastPrice);
		//mrk.highestPrice = INT_TO_DOUBLE(pRealMarketData->highPrice);
		//mrk.lowestPrice = INT_TO_DOUBLE(pRealMarketData->lowPrice);
		//mrk.highLimitPrice = INT_TO_DOUBLE(pRealMarketData->highLimited);
		//mrk.lowLimitPrice = INT_TO_DOUBLE(pRealMarketData->lowLimited);
		//mrk.openInterest = INT_TO_DOUBLE(pRealMarketData->openInterest);

		//mrk.turnover = INT_TO_DOUBLE(pRealMarketData->turnOver);
		//mrk.volume = pRealMarketData->volume;
		//mrk.tradingDay = std::to_string(pRealMarketData->date);
		//mrk.exchangeTimestamp = std::to_string(pRealMarketData->time);
		//mrk.recvTimestamp = CedarHelper::getCurTimeStamp(); //HHMMSSmmm

		//std::vector<std::string> strs;
		//std::string symb = pRealMarketData->symbol;
		////symbol is like 600000.SH use '.' to split
		//CedarHelper::stringSplit(symb, '.', strs);
		//mrk.security = strs[0];
		//mrk.exchange = strs[1];

		//std::string msg = mrk.serialize();
		//std::string chan = C_ReturnMkt;
		//pMktUpdt->msgHub.send(chan, msg);

		LOG(INFO) << "[pubMarketDataCallback][MsgHub Chan|Msg]";
	}
}

//MsgHub callback fucntion on every msg
int CatsMktUpdt::onMsg(MessageBase msg) {
	//LOG(INFO) << "[onMsg] recv: " << msg << "|" << msg;
	//if (subChanlNames.find(chan) != subChanlNames.end()) {
	//	CmdRequest cmdRequest;
	//	cmdRequest.deserializeOverwrite(msg);
	//	std::vector<std::string> symbols;

	//	if (cmdRequest.cmd == CMD_SubscribeMarketData) {
	//		CedarHelper::stringSplit(cmdRequest.args[0], ',', symbols);
	//	}

	//	for (int i = 0; i < symbols.size(); i++) {
	//		LOG(INFO) << "[onMsg] dynamic subscribe: " << symbols[i];
	//		subSingleSymbol(symbols[i]);
	//	}

	//} else {
	//	LOG(ERROR) << "[onMsg] undefined channel";
	//	return -1;
	//}

	return 0;
}

