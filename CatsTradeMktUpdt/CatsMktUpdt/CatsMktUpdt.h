#pragma once
#define _WINSOCKAPI_ 
#include "catsapi.h"
#include "catsapi_md.h"
#include "MarketDataDefinition.h"
#include "ProtoBufMsgHub.h"
#include "CatsSessionManager.h"

#include <set>
#include <tuple>
#include <string>

class CatsMktUpdt {
public:
	CatsMktUpdt();
	int subSingleSymbol(std::string sSymbol);

private:
	std::set<std::string> subChanlNames;
	CatsSessionManager csm;
	ProtoBufMsgHub msgHub;
	
	//save one id in heap space on every call
	//caution on overflow
	const char* heapSpaceStore(std::string id);
	static void PubMarketDataCallback(void* pArg);
	static void BizSubCommonCallback(void* pArg);
	int onMsg(MessageBase);
};
