#ifndef FIRST_LEVEL_ORDER_H
#define FIRST_LEVEL_ORDER_H

#include "OrderReactor.h"
#include "ProtoBufMsgHub.h"
#include "SmartOrderService.h"



class FirstLevelOrder : public OrderReactor {

public:
  FirstLevelOrder(OrderRequest&, SmartOrderService *srcv);
  virtual int onMsg(MessageBase &);

private:
  int cancelOrder();
  const static int stockMinimumQty = 100;

  enum FirstLevelOrderState {
    Init = 0,
    Ready,
    Sending,
    Sent,
    Canceling,
    CrossChase,
    TerminateCancel
  };

  FirstLevelOrderState orderState;
  int leftQty;
  double placePrice;
  std::string respAddr;
  std::string outOrderId;
};

#endif
