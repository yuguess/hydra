#ifndef ORDER_REACTOR_H
#define ORDER_REACTOR_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"

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
    orderRequest(req), service(srvc), recycleFlag(false) {}

  inline int setRecycle() { recycleFlag = true;}

  OrderRequest orderRequest;
  SmartOrderService *service;

private:
  bool recycleFlag;
};

#endif
