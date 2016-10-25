#include "stdafx.h"
#include "TdxManager.h"
#include "InterfaceOrder.h"

using namespace std;

TdxManager::TdxManager() : 
	queueTimer(ioService, boost::posix_time::seconds(frequency)),
	controlTimer(ioService, boost::posix_time::seconds(frequency)) {

	resultQueue = new char[RESULT_SIZE_QUEUEU];
	resultSendOrder = new char[RESULT_SIZE_SENDORDER];
	errInfo = new char[ERROR_INFO_SIZE];

	SH_HOLDER_CODE = "";
	SZ_HOLDER_CODE = "";
	broker = "";

	SAFE_THREASHOLD_QUEUE_MAP = 0;
	ORDER_STATE_QUEUE_MAP = 0;
	TRADE_QUANTITY_QUEUE_MAP = 0;
	TRADE_PRICE_QUEUE_MAP = 0;
	BROKER_ID_QUEUE_MAP = 0;
	TRADE_NOTIONAL_QUEUE_MAP = 0;
}

TdxManager::~TdxManager() {
	TdxApi.CloseTdx();
	int count = 0;
	if (!codeMap.empty()) {
		for (auto it = codeMap.begin(); it != codeMap.end(); it++) {
			LOG(ERROR) << "pair" << count << ":" << it->first << "," << it->second << endl;
			count++;
		}
	}
}

int TdxManager::logon()	{

	std::string ip;
	short port;
	std::string version;
	short yybID;
	std::string accountNo;
	std::string tradeAccount;
	std::string jyPassword;
	std::string txPassword;
	int logonMode;

	string stmp;

	CedarJsonConfig::getInstance().getStringByPath("Logon.IP_Address", ip);
	CedarJsonConfig::getInstance().getStringByPath("Logon.Version", version);
	CedarJsonConfig::getInstance().getStringByPath("Logon.AccountNo", accountNo);
	CedarJsonConfig::getInstance().getStringByPath("Logon.TradeAccountNo", tradeAccount);
	CedarJsonConfig::getInstance().getStringByPath("Logon.JyPassword", jyPassword);
	CedarJsonConfig::getInstance().getStringByPath("Logon.TxPassword", txPassword);
	CedarJsonConfig::getInstance().getStringByPath("Logon.Port", stmp);
	CedarJsonConfig::getInstance().getStringByPath("Logon.SH_HOLDER_CODE", SH_HOLDER_CODE);
	CedarJsonConfig::getInstance().getStringByPath("Logon.SZ_HOLDER_CODE", SZ_HOLDER_CODE);
	port = atoi(stmp.c_str());
	CedarJsonConfig::getInstance().getStringByPath("Logon.YybID", stmp);
	yybID = atoi(stmp.c_str());
	CedarJsonConfig::getInstance().getStringByPath("Logon.LogonMode", stmp);
	logonMode = atoi(stmp.c_str());
	CedarJsonConfig::getInstance().getStringByPath("Logon.Broker", broker);

	LOG(INFO) << "Login Configuration:" << endl;
	LOG(INFO) << "IP" << ": " << ip << endl;
	LOG(INFO) << "Port" << ": " << port << endl;
	LOG(INFO) << "Version" << ": " << version << endl;
	LOG(INFO) << "YybID" << ": " << yybID << endl;
	LOG(INFO) << "AccountNo" << ": " << accountNo << endl;
	LOG(INFO) << "TradeAccount" << ": " << tradeAccount << endl;
	LOG(INFO) << "JyPassword" << ": " << jyPassword << endl;
	LOG(INFO) << "TxPassword" << ": " << txPassword << endl;
	LOG(INFO) << "SH_HOLDER_CODE" << ": " << SH_HOLDER_CODE << endl;
	LOG(INFO) << "SZ_HOLDER_CODE" << ": " << SZ_HOLDER_CODE << endl;
	LOG(INFO) << "LogonMode" << ": " << logonMode << endl;

	if (-1 == setBrokerConfig()) {
		LOG(INFO) << "We can't recogonize the broker name!";
		getchar();
		exit(-1);
	}

	clientID = TdxApi.Logon(const_cast<char*>(ip.c_str()), port, const_cast<char*>(version.c_str()), yybID, logonMode,
		const_cast<char*>(accountNo.c_str()), const_cast<char*>(tradeAccount.c_str()), const_cast<char*>(jyPassword.c_str()), const_cast<char*>( txPassword.c_str() ), errInfo);
	
	if (clientID == -1) {
		LOG(ERROR) << "Logon failed. ";
		LOG(INFO) << errInfo;
		getchar();
		exit(-1);
	}

	LOG(INFO) << "Login successfully!";

	return clientID;
}

void TdxManager::initialize() {
	TdxApi.OpenTdx();
}

int TdxManager::setBrokerConfig() {
	if (broker == "GUOJIN") {
		SAFE_THREASHOLD_QUEUE_MAP = GUOJIN_SAFE_THREASHOLD_QUEUE_MAP;
		ORDER_STATE_QUEUE_MAP = GUOJIN_ORDER_STATE_QUEUE_MAP;
		TRADE_QUANTITY_QUEUE_MAP = GUOJIN_TRADE_QUANTITY_QUEUE_MAP;
		TRADE_PRICE_QUEUE_MAP = GUOJIN_TRADE_PRICE_QUEUE_MAP;
		BROKER_ID_QUEUE_MAP = GUOJIN_BROKER_ID_QUEUE_MAP;
		brokerCode = BROKER_CODE_GUOJIN;
	}
	else if (broker == "ZHONGXIN") {
		SAFE_THREASHOLD_QUEUE_MAP = ZHONGXIN_SAFE_THREASHOLD_QUEUE_MAP;
		ORDER_STATE_QUEUE_MAP = ZHONGXIN_ORDER_STATE_QUEUE_MAP;
		TRADE_QUANTITY_QUEUE_MAP = ZHONGXIN_TRADE_QUANTITY_QUEUE_MAP;
		TRADE_PRICE_QUEUE_MAP = ZHONGXIN_TRADE_PRICE_QUEUE_MAP;
		BROKER_ID_QUEUE_MAP = ZHONGXIN_BROKER_ID_QUEUE_MAP;
		brokerCode = BROKER_CODE_ZHONGXIN;
	}
	else if (broker == "GTJA") {
		SAFE_THREASHOLD_QUEUE_MAP = GTJA_SAFE_THREASHOLD_QUEUE_MAP;
		ORDER_STATE_QUEUE_MAP = GTJA_ORDER_STATE_QUEUE_MAP;
		TRADE_QUANTITY_QUEUE_MAP = GTJA_TRADE_QUANTITY_QUEUE_MAP;
		TRADE_PRICE_QUEUE_MAP = GTJA_TRADE_PRICE_QUEUE_MAP;
		BROKER_ID_QUEUE_MAP = GTJA_BROKER_ID_QUEUE_MAP;
		brokerCode = BROKER_CODE_GTJA;
	}
	else if (broker == "TEST") {
		SAFE_THREASHOLD_QUEUE_MAP = TEST_SAFE_THREASHOLD_QUEUE_MAP;
		ORDER_STATE_QUEUE_MAP = TEST_ORDER_STATE_QUEUE_MAP;
		TRADE_QUANTITY_QUEUE_MAP = TEST_TRADE_QUANTITY_QUEUE_MAP;
		TRADE_PRICE_QUEUE_MAP = TEST_TRADE_PRICE_QUEUE_MAP;
		BROKER_ID_QUEUE_MAP = TEST_BROKER_ID_QUEUE_MAP;
		brokerCode = BROKER_CODE_TEST;
	}
	else if (broker == "HW") {
		SAFE_THREASHOLD_QUEUE_MAP = HW_SAFE_THREASHOLD_QUEUE_MAP;
		ORDER_STATE_QUEUE_MAP = HW_ORDER_STATE_QUEUE_MAP;
		TRADE_QUANTITY_QUEUE_MAP = HW_TRADE_QUANTITY_QUEUE_MAP;
		TRADE_PRICE_QUEUE_MAP = HW_TRADE_PRICE_QUEUE_MAP;
		BROKER_ID_QUEUE_MAP = HW_BROKER_ID_QUEUE_MAP;
		//brokerCode = BROKER_CODE_HW;
	}
	else if (broker == "YINHE") {
		SAFE_THREASHOLD_QUEUE_MAP = YH_SAFE_THREASHOLD_QUEUE_MAP;
		ORDER_STATE_QUEUE_MAP = YH_ORDER_STATE_QUEUE_MAP;
		TRADE_QUANTITY_QUEUE_MAP = YH_TRADE_QUANTITY_QUEUE_MAP;
		TRADE_PRICE_QUEUE_MAP = YH_TRADE_PRICE_QUEUE_MAP;
		BROKER_ID_QUEUE_MAP = YH_BROKER_ID_QUEUE_MAP;
		//brokerCode = BROKER_CODE_HW;
	}
	else if (broker == "ZHONGTAI") {
		SAFE_THREASHOLD_QUEUE_MAP = ZT_SAFE_THREASHOLD_QUEUE_MAP;
		ORDER_STATE_QUEUE_MAP = ZT_ORDER_STATE_QUEUE_MAP;
		TRADE_QUANTITY_QUEUE_MAP = ZT_TRADE_QUANTITY_QUEUE_MAP;
		TRADE_PRICE_QUEUE_MAP = ZT_TRADE_PRICE_QUEUE_MAP;
		BROKER_ID_QUEUE_MAP = ZT_BROKER_ID_QUEUE_MAP;
		//brokerCode = BROKER_CODE_HW;
	}
	else {
		return -1;
	}
	return 1;
}

int TdxManagersetRespondMessageBase(ResponseMessage &rmsg, list<InterfaceOrder>::iterator &bufIter, map<string, ResponseMessage>::iterator &stateIter) {
	rmsg.set_id("rep");
	rmsg.set_ref_id(bufIter->req.id());
	rmsg.set_code(bufIter->req.code());
	rmsg.set_symbol(bufIter->req.symbol());
	rmsg.set_exchange(bufIter->req.exchange());
	rmsg.set_price(bufIter->req.limit_price());
	rmsg.set_buy_sell(bufIter->req.buy_sell());
	rmsg.set_trade_quantity(stateIter->second.trade_quantity() - bufIter->tradedQuantity);
	//	rmsg.set_error_msg(errInfo);
	//	rmsg.set_exchange_timestamp("none");
	//	rmsg.set_recv_timestamp("none");
	return 0;
}

int TdxManager::createStateMap(vector<string> &orderInLine, map<string, ResponseMessage> &stateMap) {
	vector<string> inLineData;
	ResponseMessage tmpRespInfo;
	for (auto iter = orderInLine.begin(); iter != orderInLine.end(); iter++) {
		//boost::split(inLineData, (*iter), boost::is_any_of("\t\0"), boost::token_compress_on);
		inLineData = split_line_own(const_cast<char*>(iter->c_str()));
		
		if (inLineData.size() < SAFE_THREASHOLD_QUEUE_MAP) {
			LOG(WARNING) << "corrupted data line " << std::endl;
			return -1;
		}
		//LOG(INFO) << inLineData[BROKER_ID_QUEUE_MAP];
		tmpRespInfo.set_code(inLineData[ORDER_STATE_QUEUE_MAP]);

		//the trade quantity here is the real traded quantity, not the requeststed quantity
		tmpRespInfo.set_trade_quantity(atoi(const_cast<char *>(inLineData[TRADE_QUANTITY_QUEUE_MAP].c_str())));
		tmpRespInfo.set_price(stringToDouble(inLineData[TRADE_PRICE_QUEUE_MAP]));

		//LOG(INFO) << "broker id:" << inLineData[BROKER_ID_QUEUE_MAP];
		stateMap.insert(pair<string, ResponseMessage>(inLineData[BROKER_ID_QUEUE_MAP], tmpRespInfo));
	}
	return 0;
}

int TdxManager::removableStateProcess(list<InterfaceOrder>::iterator &bufIter, 
	map<string, ResponseMessage>::iterator &stateIter, ResponseMessage &rmsg) {
	
	string sendAddress = bufIter->req.response_address();
	string buy_sell = "";

	if (codeMap.find(bufIter->req.id()) == codeMap.end()) {
		LOG(WARNING) << "erase code map, but can't find its mapping id ";
		//return 0;
	}

	if (stateIter->second.code() == "已成" || stateIter->second.code() == "全部成交") {
		rmsg.set_type(TYPE_TRADE);
		TdxManagersetRespondMessageBase(rmsg, bufIter, stateIter);
		rmsg.set_error_code(DEAL);
		rmsg.set_price(stateIter->second.price());

		msgHub.pushMsg(sendAddress, ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rmsg));

		LogHelper::logObject(rmsg);
	}
	else if (stateIter->second.code() == "已撤" || stateIter->second.code() == "场内撤单") {

		rmsg.set_type(TYPE_CANCEL_ORDER_CONFIRM);
		TdxManagersetRespondMessageBase(rmsg, bufIter, stateIter);
		rmsg.set_error_code(CANCELLED);
		rmsg.set_price(stateIter->second.price());

		msgHub.pushMsg(sendAddress, ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rmsg));

		LogHelper::logObject(rmsg);
	} else if (stateIter->second.code() == "部撤") {

		rmsg.set_type(TYPE_TRADE);
		TdxManagersetRespondMessageBase(rmsg, bufIter, stateIter);
		rmsg.set_error_code(PARTIALLY_DEAL);
		rmsg.set_price(stateIter->second.price());

		msgHub.pushMsg(sendAddress, ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rmsg));
		LogHelper::logObject(rmsg);

		rmsg.set_type(TYPE_CANCEL_ORDER_CONFIRM);
		rmsg.set_error_code(PARTIALLY_CANCELLED);

		msgHub.pushMsg(sendAddress, ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rmsg));
		LogHelper::logObject(rmsg);
	} else if (stateIter->second.code() == "废单") {

		rmsg.set_code("bad order");
		rmsg.set_type(TYPE_ERROR);

		TdxManagersetRespondMessageBase(rmsg, bufIter, stateIter);
		rmsg.set_error_code(BAD_ORDER_ERROR);
		msgHub.pushMsg(sendAddress, ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rmsg));

		LogHelper::logObject(rmsg);
	}

	return 0;
}

int TdxManager::reserveStateProcess(list<InterfaceOrder>::iterator &bufIter,
	map<string, ResponseMessage>::iterator &stateIter, ResponseMessage &rmsg) {
	string sendAddress = bufIter->req.response_address();
	rmsg.set_type(TYPE_TRADE);
	TdxManagersetRespondMessageBase(rmsg, bufIter, stateIter);
	rmsg.set_error_code(PARTIALLY_DEAL);

	if (bufIter->tradedQuantity != stateIter->second.trade_quantity()) {
		msgHub.pushMsg(sendAddress, ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rmsg));
		LogHelper::logObject(rmsg);
		LOG(INFO) << "traded:" << stateIter->second.trade_quantity() - bufIter->tradedQuantity;

		bufIter->tradedQuantity = stateIter->second.trade_quantity();
		
	}
	return 0;
}

void TdxManager::queueing(const boost::system::error_code &error) {
	int ret = queue();
	if (ret == 0) {
		registerNextQueue();
	}
	else {
		LOG(INFO) << "queueing failed";
	}
}

int TdxManager::queue()	{
	clock_t tt;
	InterfaceOrder tmpBuf;
	string bufOrderID;
	int ret = 0;

	map<string, ResponseMessage> stateMap;
	map<string, ResponseMessage>::iterator stateIter;
	list<InterfaceOrder>::iterator tmpBufNextIter;
	vector<string> orderInLine;

	if (orderBuf.empty()) {
		return 0;
	}

	TdxApi.QueryData(clientID, QUE_CODE_CHECK_ORDER, resultQueue, errInfo);

	//boost::split(orderInLine, resultQueue, boost::is_any_of("\n\0"), boost::token_compress_on);
	orderInLine = split_data_own(resultQueue);

	if (orderInLine.empty()) {
		LOG(ERROR) << "Decoding Error:";
		return 0;
	}
	
	if ((ret = createStateMap(orderInLine, stateMap)) == -1) {
		LOG(INFO) << "create map failed";
		LOG(WARNING) << resultQueue;
		return 0;
	}

	for (auto bufIter = orderBuf.begin(); bufIter != orderBuf.end();) {
		tmpBuf = *bufIter;
		bufOrderID = bufIter->orderId;
		//LOG(INFO) << "debug:" << bufIter->orderId;
		stateIter = stateMap.find(bufOrderID);
		if (stateIter == stateMap.end()) {
			bufIter++;
			LOG(INFO) << "id in monitor not found in broker book!";
			continue;
		}

		ResponseMessage rmsg;
		string hydraID = bufIter->req.id();
		string sendAddress = bufIter->req.response_address();
		
		if (stateIter->second.code() == "已成" | stateIter->second.code() == "已撤" | stateIter->second.code() == "场内撤单" |
			stateIter->second.code() == "部撤" | stateIter->second.code() == "已成" | 
			stateIter->second.code() == "废单" | stateIter->second.code() == "全部成交") {

			if ((ret = removableStateProcess(bufIter, stateIter, rmsg)) == -1) {
				LOG(INFO) << "remove process failed!";
				return 0;
			}

			//recycle process 
			codeMap.erase(bufIter->req.id());
			bufIter = orderBuf.erase(bufIter);

		} else if (stateIter->second.code() == "部成") {
			reserveStateProcess(bufIter, stateIter, rmsg);
			bufIter++;
		} else {
			bufIter++;
		}
	}

	return 0;
}

void TdxManager::registerNextQueue() {
	queueTimer.expires_at(queueTimer.expires_at() + boost::posix_time::seconds(frequency));
	queueTimer.async_wait(boost::bind(&TdxManager::queueing, this, boost::asio::placeholders::error));
}

int TdxManager::startService() {
	ProtoBufHelper::setupProtoBufMsgHub(msgHub);
	msgHub.registerCallback(std::bind(&TdxManager::onMsg, this, std::placeholders::_1));

	queueTimer.async_wait(boost::bind(&TdxManager::queueing, this, boost::asio::placeholders::error));
	LOG(INFO) << "service start!";
	
	ioService.run();

	return 0;
}

int TdxManager::onMsg(MessageBase msg) {
	LOG(INFO) << "ORDER GET!";
	int category, priceType = 0;
	string shareHolderCode = "";
	string stockCode = "";
	float price = 0.0;
	int quantity = 0;
	string orderNum = "";
	string exchangeID = "";
	
	ResponseMessage rmsg;

	LOG(INFO) << "Checking Order Request!";
	if (msg.type() != TYPE_ORDER_REQUEST) {
		LOG(WARNING) << "recv invalid msg type " << msg.type();
		return -1;
	}

	LOG(INFO) << "Order request type checked!";
	InterfaceOrder tmp;
	LOG(INFO) << "unwrap order req message!";
	OrderRequest ordReq = ProtoBufHelper::unwrapMsg<OrderRequest>(msg);
	LOG(INFO) << "req message wrapped!";

	LOG(INFO) << "basic req setting start!";
	tmp.setRequest(ordReq);

	category = ordReq.buy_sell();
	shareHolderCode = InterfaceOrder::shareHolderCode;
	stockCode = ordReq.code();
	price = ordReq.limit_price();
	quantity = ordReq.trade_quantity();

	if (ordReq.exchange() == SHSE) {
		shareHolderCode = SH_HOLDER_CODE;
		exchangeID = SH_EXC_API;
	} else {
		shareHolderCode = SZ_HOLDER_CODE;
		exchangeID = SZ_EXC_API;
	}
	LOG(INFO) << "basic req setting end!";
	//LOG(INFO) << "RECEIVE ORDER!; " << ordReq.DebugString();
	LogHelper::logObject(ordReq);
	
	string sendAddress = ordReq.response_address();
	
	if (ordReq.type() == TYPE_LIMIT_ORDER_REQUEST) {
		priceType = PRICE_TYPE_LIMIT;
		LOG(INFO) << "debug" << shareHolderCode;
		TdxApi.SendOrder(clientID, category, priceType, (char*)shareHolderCode.c_str(), (char*)stockCode.c_str(), price, quantity, resultSendOrder, errInfo);

		if (errInfo[0] !='\0') {

			rmsg.set_type(TYPE_ERROR);
			rmsg.set_id("rep");
			rmsg.set_ref_id(ordReq.id());
			rmsg.set_code(ordReq.code());
			rmsg.set_symbol(ordReq.symbol());
			rmsg.set_exchange(ordReq.exchange());
			rmsg.set_price(ordReq.limit_price());
			rmsg.set_buy_sell(ordReq.buy_sell());
			rmsg.set_trade_quantity(tmp.tradedQuantity);
			rmsg.set_error_code(SEND_ERROR);
		//	rmsg.set_error_msg(errInfo);
		//	rmsg.set_exchange_timestamp("none");
		//	rmsg.set_recv_timestamp("none");
			LOG(ERROR) << errInfo;
			msgHub.pushMsg(sendAddress, ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rmsg));

			LogHelper::logObject(rmsg);

			return 0;
		}

		orderNum = getOrderID(resultSendOrder);
		tmp.setID(orderNum);
		orderBuf.push_back(tmp);
		codeMap.insert(pair<string, string>(ordReq.id(), orderNum));
		LOG(INFO) << "***IDMap:" + ordReq.id() + "," + orderNum;

		rmsg.set_type(TYPE_NEW_ORDER_CONFIRM);
		rmsg.set_id("rep");
		rmsg.set_ref_id(ordReq.id());
		rmsg.set_code(ordReq.code());
		rmsg.set_symbol(ordReq.symbol());
		rmsg.set_exchange(ordReq.exchange());
		rmsg.set_price(ordReq.limit_price());
		rmsg.set_buy_sell(ordReq.buy_sell());
		rmsg.set_trade_quantity(tmp.tradedQuantity);
		rmsg.set_error_code(NONE_ERROR);
		//	rmsg.set_error_msg(errInfo);
		//	rmsg.set_exchange_timestamp("none");
		//	rmsg.set_recv_timestamp("none");

		msgHub.pushMsg(sendAddress, ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rmsg));

		LogHelper::logObject(rmsg);
	} else if (ordReq.type() == TYPE_MARKET_ORDER_REQUEST) {
		priceType = PRICE_TYPE_MARKET;

		TdxApi.SendOrder(clientID, category, priceType, (char*)shareHolderCode.c_str(), (char*)stockCode.c_str(), price, quantity, resultSendOrder, errInfo);

		if (errInfo[0] != '\0') {

			rmsg.set_type(TYPE_ERROR);
			rmsg.set_id("rep");
			rmsg.set_ref_id(ordReq.id());
			rmsg.set_code(ordReq.code());
			rmsg.set_symbol(ordReq.symbol());
			rmsg.set_exchange(ordReq.exchange());
			rmsg.set_price(ordReq.limit_price());
			rmsg.set_buy_sell(ordReq.buy_sell());
			rmsg.set_trade_quantity(tmp.tradedQuantity);
			rmsg.set_error_code(SEND_ERROR);
			//	rmsg.set_error_msg(errInfo);
			//	rmsg.set_exchange_timestamp("none");
			//	rmsg.set_recv_timestamp("none");
			LOG(ERROR) << errInfo;

			msgHub.pushMsg(sendAddress, ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rmsg));

			LogHelper::logObject(rmsg);
			return 0;
		}

		orderNum = getOrderID(resultSendOrder);
		tmp.setID(orderNum);
		
		orderBuf.push_back(tmp);
		codeMap.insert(pair<string, string>(ordReq.id(), orderNum));

		rmsg.set_type(TYPE_NEW_ORDER_CONFIRM);
		rmsg.set_id("rep");
		rmsg.set_ref_id(ordReq.id());
		rmsg.set_code(ordReq.code());
		rmsg.set_symbol(ordReq.symbol());
		rmsg.set_exchange(ordReq.exchange());
		rmsg.set_price(ordReq.limit_price());
		rmsg.set_buy_sell(ordReq.buy_sell());
		rmsg.set_trade_quantity(tmp.tradedQuantity);
		rmsg.set_error_code(NONE_ERROR);
		//	rmsg.set_error_msg(errInfo);
		//	rmsg.set_exchange_timestamp("none");
		//	rmsg.set_recv_timestamp("none");

		LogHelper::logObject(rmsg);
	} else if (ordReq.type() == TYPE_CANCEL_ORDER_REQUEST) {
		string ctmp = ordReq.cancel_order_id();
		if (codeMap.find(ctmp) == codeMap.end()) {
			rmsg.set_type(TYPE_ERROR);
			rmsg.set_id("rep");
			rmsg.set_ref_id(ordReq.id());
			rmsg.set_code(ordReq.code());
			rmsg.set_symbol(ordReq.symbol());
			rmsg.set_exchange(ordReq.exchange());
			rmsg.set_price(ordReq.limit_price());
			rmsg.set_buy_sell(ordReq.buy_sell());
			rmsg.set_trade_quantity(tmp.tradedQuantity);
			rmsg.set_error_code(SEND_ERROR);
			//	rmsg.set_error_msg(errInfo);
			//	rmsg.set_exchange_timestamp("none");
			//	rmsg.set_recv_timestamp("none");

			msgHub.pushMsg(sendAddress, ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rmsg));


			LogHelper::logObject(rmsg);
			LOG(INFO) << "Order no longer in monitor";
			return 1;
		}

		string otmp = codeMap[ctmp];

		TdxApi.CancelOrder(clientID, (char*)exchangeID.c_str(), (char*)otmp.c_str(), resultSendOrder, errInfo);

		if (errInfo[0] != '\0') {
			rmsg.set_type(TYPE_ERROR);
			rmsg.set_id("rep");
			rmsg.set_ref_id(ordReq.id());
			rmsg.set_code(ordReq.code());
			rmsg.set_symbol(ordReq.symbol());
			rmsg.set_exchange(ordReq.exchange());
			rmsg.set_price(ordReq.limit_price());
			rmsg.set_buy_sell(ordReq.buy_sell());
			rmsg.set_trade_quantity(tmp.tradedQuantity);
			rmsg.set_error_code(CANCEL_ERROR);
			//	rmsg.set_error_msg(errInfo);
			//	rmsg.set_exchange_timestamp("none");
			//	rmsg.set_recv_timestamp("none");
			LOG(ERROR) << errInfo;
			msgHub.pushMsg(sendAddress, ProtoBufHelper::wrapMsg(TYPE_RESPONSE_MSG, rmsg));
		}
	}

	return 0;
}

string TdxManager::getOrderID(char *result) {

	vector<string> strvec;
	vector<string> data;
	string out;

	boost::split(strvec, result, boost::is_any_of("\n\0"), boost::token_compress_on);

	vector<string>::iterator iter = strvec.begin();
	iter++;

	boost::split(data, *iter, boost::is_any_of("\t\0"), boost::token_compress_on);

	vector<string>::iterator iter1 = data.begin(), iter2 = data.end();
	int count = 0;
	out = *iter1;

	return out;
}

double TdxManager::stringToDouble(const string& str) {
	istringstream iss(str);
	double num;
	iss >> num;
	return num;
}

int TdxManager::stringToInt(const string& str) {
	istringstream iss(str);
	int num;
	iss >> num;
	return num;
}
