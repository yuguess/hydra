#ifndef ORDER_REACTOR_H
#define ORDER_REACTOR_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "CedarHelper.h"
#include "LogHelper.h"

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

  inline void setRecycle() {recycleFlag = true;}

  bool logStatusInfo(MarketUpdate &mkt) {
    AppStatusMsg statusMsg;
    statusMsg.set_type(APP_STATUS_MSG);
    statusMsg.set_batch_id(orderRequest.batch_id());
    statusMsg.set_alg_order_id(orderRequest.alg_order_id());
    statusMsg.add_keys("ReferencePrice");
    statusMsg.add_values(
        std::to_string(0.5 * mkt.bid_price(0) + 0.5 * mkt.ask_price(0)));

    LogHelper::logObject(statusMsg);
    return true;
  }

  OrderRequest orderRequest;
  SmartOrderService *service;

private:
  bool recycleFlag;
};

#endif
