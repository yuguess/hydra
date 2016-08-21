#ifndef SMART_ORDER_SERVICE_H
#define SMART_ORDER_SERVICE_H

#include "ProtoBufMsgHub.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "CedarHelper.h"
#include "OrderReactor.h"

class SmartOrderService {
public:
  SmartOrderService();
  int sendRequest(std::string, OrderRequest&);

private:
  int subscribeTicker(std::string, ExchangeType);
  int onMsg(MessageBase msg);
  int addReactorToMktDriver(std::string&, std::shared_ptr<OrderReactor>&);
  int onMktUpdate(MarketUpdate&, MessageBase&);
  int onOrderRequest(OrderRequest&);
  int onResponseMsg(ResponseMessage&, MessageBase&);

  std::string stkDataAddr;
  std::string stkBoardcastAddr;
  std::string futuresDataAddr;
  std::string futuresBoardcastAddr;

  ProtoBufMsgHub msgHub;

  std::map<std::string,
    std::list<std::shared_ptr<OrderReactor>>> mktDriver;
  std::map<std::string, std::shared_ptr<OrderReactor>> rspDriver;
  std::map<std::string, std::shared_ptr<OrderReactor>> orderReactorIDtoObj;
  std::map<std::string, std::string> accToAddr;
};

#endif
