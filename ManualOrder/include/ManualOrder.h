#ifndef MANUAL_ORDER_H
#define MANUAL_ORDER_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "CedarHelper.h"
using namespace std;


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
  std::string queryID();
  std::string queryRefID();
  std::string queryCode();
  std::string queryChan();
  std::string queryCancelID();
  //ReturnType queryRtnType();
  RequestType queryOrdType();
  PositionDirection queryOrdPosition();
  //OrderAction queryOrdAction();
  int onMsg(MessageBase);
  int onUpdateMsg(MessageBase);
  int onMsgTest(MessageBase);

  void updateTestBoardcast();
  
  std::string sendAddr;
  std::string pubAddr;
};

#endif
