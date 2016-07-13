#ifndef MANUAL_ORDER_H
#define MANUAL_ORDER_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "CedarHelper.h"

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

private:
  ProtoBufMsgHub msgHub;

  int queryAction();
  int queryEnterOrder();
  TradeDirection querySide();
  double queryPrice();
  int queryOrderQty();
  int queryCancelOrder();
  std::string queryAccount();
  std::string queryID();
  std::string queryRefID();
  std::string queryCode();
  std::string queryCancelID();
  RequestType queryOrdType();
  PositionDirection queryOrdPosition();
  ExchangeType queryExchange();
  int queryDataRequest();

  int onMsg(MessageBase);

  std::vector<DataServer> dataServers;
  std::vector<TradeServer> tradeServers;
};

#endif
