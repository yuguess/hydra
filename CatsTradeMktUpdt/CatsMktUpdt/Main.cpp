#include "CatsMktUpdt.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "CedarLogging.h"

int main() {
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	
	CedarLogging::init("CatsMktUpdt");
	CedarJsonConfig::getInstance().loadConfigFile("CatsMktUpdt.json");

	//sub Market data
	CatsMktUpdt mdu;
	LOG(INFO) << "CatsMktUpdt ctor complete";
	//std::vector<std::string> codes;
	//std::vector<std::string> exchanges;
	//CedarJsonConfig::getInstance().getStringArrayWithTag(codes, "Ticker", "Symbol");
	//CedarJsonConfig::getInstance().getStringArrayWithTag(exchanges, "Ticker", "Exchange");
	
	//for (unsigned i = 0; i < codes.size(); i++) {
	//	std::string ticker = codes[i] + "." + exchanges[i];
	//	LOG(INFO) << "init subscribe: " << ticker;
	//	mdu.subSingleSymbol(ticker);
	//}

	getchar();
	return 0;
}