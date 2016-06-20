#ifndef MANUAL_ORDER_H
#define MANUAL_ORDER_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "CedarHelper.h"

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
  std::string queryID();
  std::string queryRefID();
  std::string queryCode();
  std::string queryChan();
  //ReturnType queryRtnType();
  RequestType queryOrdType();
  PositionDirection queryOrdPosition();
  //OrderAction queryOrdAction();
  int onMsg(MessageBase);

  std::string sendAddr;
};

#endif
