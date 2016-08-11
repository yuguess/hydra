#ifndef FIRST_LEVEL_ORDER_H
#define FIRST_LEVEL_ORDER_H

#include "OrderReactor.h"
#include "ProtoBufMsgHub.h"
#include "SmartOrderService.h"

enum OrderState {
  Ready = 0,
  Sending,
  Sent,
  Canceling,
  CrossChase,
  TerminateCancel
};

class FirstLevelOrder : public OrderReactor {

public:
  FirstLevelOrder(OrderRequest&, SmartOrderService *srcv);
  virtual int onMsg(MessageBase &);

private:
  int cancelOrder();
  const static int stockMinimumQty = 100;

  OrderState orderState;
  int leftQty;
  double placePrice;
  std::string respAddr;
  std::string outOrderId;
};

#endif
