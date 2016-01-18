#include "CatsMktUpdt.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"

int main() {
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	
	CedarHelper::initGlog("CatsMktUpdt");
	CedarJsonConfig::getInstance().loadConfigFile("./CatsMktUpdt.json");

	//sub Market data
	CatsMktUpdt mdu;

	//const libconfig::Setting& Symbols = ConfigGetSetting("Symbols");
	//for (unsigned i = 0; i < Symbols.getLength(); i++) {
	//	const char *s = Symbols[i];
	//	LOG(INFO) << "init subscribe: " << s;
	//	mdu.subSingleSymbol(s);
	//}

	std::vector<std::string> codes;
	std::vector<std::string> exchanges;
	CedarJsonConfig::getInstance().getStringArrayWithTag(codes, "Ticker", "Symbol");
	CedarJsonConfig::getInstance().getStringArrayWithTag(exchanges, "Ticker", "Exchange");
	
	for (unsigned i = 0; i < codes.size(); i++) {
		std::string ticker = codes[i] + "." + exchanges[i];
		LOG(INFO) << "init subscribe: " << ticker;
		mdu.subSingleSymbol(ticker);
	}

	getchar();
	return 0;
}