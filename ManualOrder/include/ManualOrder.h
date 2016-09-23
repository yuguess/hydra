#ifndef MANUAL_ORDER_H
#define MANUAL_ORDER_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "CedarHelper.h"
using namespace std;


struct DataServer {
  std::string name;
  std::string serverAddr;
  std::string boardcastAddr;
};

struct TradeServer {
  std::string name;
  std::string address;
};

class ManualOrder {

public:
  ManualOrder();
  void run();
  void autorun();
private:
  ProtoBufMsgHub msgHub;

  int queryEnterOrder();
  TradeDirection querySide();
  double queryPrice();
  int queryOrderQty();
  std::string queryID();
  std::string queryRefID();
  std::string queryCode();
  std::string queryChan();
  std::string queryCancelID();

  int queryAlgOrder();
  RequestType queryOrdType();
  RequestType queryAlgOrdType();

  PositionDirection queryOrdPosition();
  ExchangeType queryExchange();
  int queryDataRequest();

  int setOrderReqAccount(OrderRequest&);

  int onMsg(MessageBase);
  int onUpdateMsg(MessageBase);
  int onMsgTest(MessageBase);

  void updateTestBoardcast();
  std::string sendAddr;
  std::string pubAddr;
  std::vector<DataServer> dataServers;
  std::vector<TradeServer> tradeServers;
  std::map<std::string, std::string> serverAddrMap;
};

#endif
