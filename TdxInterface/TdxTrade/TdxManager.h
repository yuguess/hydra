#ifndef TDXW_MANAGER_H
#define TDXW_MANAGER_H

#include <iostream>
#include <fstream>
#include <atomic>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "TdxApi.h"
#include "stdafx.h"

#include "InterfaceOrder.h"

#include "TdxAutoLib.h"

#include "Parameters.h"
#include <vector>
#include <list>
#include "ProtoBufMsgHub.h"
#include "CedarHelper.h"
#include "CedarStringHelper.h"
#include "LogHelper.h"
#include "QueueMap.h"


#define ELPP_THREAD_SAFE

using namespace std;

class TdxManager {
public:
	TdxManager();
	~TdxManager();

	int logon();
	void initialize();
	int startService();
	ProtoBufMsgHub msgHub;

private:
	const static int initLatency = 1;
	const static int frequency = 1;

	string getOrderID(char *result);
	int onMsg(MessageBase msg);

	void queueing(const boost::system::error_code &error);
	int queue();
	void registerNextQueue();

	int clientID;
	CTdxApi TdxApi;

	//don't make it local, it will cause stack overflow
	char *resultQueue;
	char *resultSendOrder;
	char *errInfo;

	boost::asio::io_service ioService;
	boost::asio::deadline_timer queueTimer;
	boost::asio::deadline_timer controlTimer;
	
	int setRespondMessageBase(ResponseMessage &rmsg, list<InterfaceOrder>::iterator &bufIter);
	int createStateMap(vector<string> &orderInLine, map<string, ResponseMessage> &stateMap);
	int removableStateProcess(list<InterfaceOrder>::iterator &bufIter,
		map<string, ResponseMessage>::iterator &stateIter, ResponseMessage &rmsg);
	int reserveStateProcess(list<InterfaceOrder>::iterator &bufIter,
		map<string, ResponseMessage>::iterator &stateIter, ResponseMessage &rmsg);
	int badOrderOProcess(list<InterfaceOrder>::iterator &bufIter,
		map<string, ResponseMessage>::iterator &stateIter, ResponseMessage &rmsg);
	int setBrokerConfig();

	int test();
	
	list<InterfaceOrder> orderBuf;
	map<string, string> codeMap;

	double stringToDouble(const string& str);
	int stringToInt(const string& str);

	string SH_HOLDER_CODE;
	string SZ_HOLDER_CODE;

	string broker;
	int brokerCode;

	int SAFE_THREASHOLD_QUEUE_MAP;
	int ORDER_STATE_QUEUE_MAP;
	int TRADE_QUANTITY_QUEUE_MAP;
	int TRADE_PRICE_QUEUE_MAP;
	int BROKER_ID_QUEUE_MAP;
	int TRADE_NOTIONAL_QUEUE_MAP;
	
};

#endif