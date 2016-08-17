#ifndef ORDER_REACTOR_H
#define ORDER_REACTOR_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "CedarHelper.h"

class SmartOrderService;

class OrderReactor {
public:
  std::string getOrderReactorID() const {
    return orderRequest.id();
  }

  bool isRecycle() {
    return recycleFlag;
  }

  virtual int onMsg(MessageBase &msg) = 0;

protected:
  OrderReactor(OrderRequest &req, SmartOrderService *srvc) :
    orderRequest(req), service(srvc), recycleFlag(false) {
    orderRequest.set_alg_order_id(CedarHelper::getOrderId());
  }

  inline void setRecycle() {recycleFlag = true;}

  OrderRequest orderRequest;
  SmartOrderService *service;

private:
  bool recycleFlag;
};

#endif
