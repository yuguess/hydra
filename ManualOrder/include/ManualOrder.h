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

  void test1(); 
  void test2();
  void sendSingleOrder(string id, string code, TradeDirection buy_sell, int quantity); 
  void cancelSingleOrder(string id, string ref_id);
  
  void updataTest();


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
<<<<<<< HEAD
  std::string queryChan();
  std::string queryCancelID();
  //ReturnType queryRtnType();
=======
  std::string queryCancelID();
>>>>>>> 854bf1ca6b236981047b28d13d22107336e77721
  RequestType queryOrdType();
  PositionDirection queryOrdPosition();
  ExchangeType queryExchange();
  int queryDataRequest();

  int onMsg(MessageBase);
  int onUpdateMsg(MessageBase);
  int onMsgTest(MessageBase);

<<<<<<< HEAD
  void updateTestBoardcast();
  
  std::string sendAddr;
  std::string pubAddr;
=======
  std::vector<DataServer> dataServers;
  std::vector<TradeServer> tradeServers;
>>>>>>> 854bf1ca6b236981047b28d13d22107336e77721
};

#endif
