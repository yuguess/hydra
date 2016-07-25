#ifndef SMART_ORDER_H
#define SMART_ORDER_H

#include "OrderReactor.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"

class SmartOrderService;

class SmartOrder : public OrderReactor {

public:
  SmartOrder(OrderRequest &req, SmartOrderService *srvc):
    OrderReactor(req, srvc) {}
  int onMsg(MessageBase &);

private:
  int onMktUpdate(MarketUpdate&);
  int onOrderRequest(OrderRequest&);
  int onResponseMsg(ResponseMessage&);

};

#endif
