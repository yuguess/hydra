#ifndef SMALL_ORDER_H
#define SMALL_ORDER_H

#include <chrono>
#include "OrderReactor.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "SmallOrder.h"

class SmallOrder : public OrderReactor {

public:
  SmallOrder(OrderRequest &, SmartOrderService*);
  int onMsg(MessageBase &);

private:
  //recalculate fair price every 5 mins 
  const static int refreshTimePeriod = 7 * 60;
  const static int refreshTimeDelta = 70;
  const static int maxTimePeriod = 90 * 60;
  const static int stockMinimumQty = 100;
  const static int touchSizePercent = 80;
  const static int maxNumOrder = 100;
  const static int mmHSpreadMultiple = 2.5;

  enum OrderState {
    Init = 0,
    Ready,
    Sending,
    Sent,
    Canceling,
    TerminateCancel
  };

  enum OrderLeg {
    Starting = 0,
    Passive,
    Aggressive,
  };

  enum SwitchSignal {
    Initial = 0,
    TimeExpired,
    FinishPhase
  };
  
  int onMktUpdate(MarketUpdate&);
  int onOrderRequest(OrderRequest&);
  int onResponseMsg(ResponseMessage&);
  int updateLeg(SwitchSignal);

  double calHalfSpread(MarketUpdate &);
  double calPlacePrice(MarketUpdate&);
  int calPlaceQty(MarketUpdate&);

  bool isValidMkt(MarketUpdate&);
  bool isValidOrder(OrderRequest&);
  bool isMaxTimeExpired();
  bool isTimeExpired();
  bool isMktMoved(double);

  int placeOrder(MarketUpdate&);
  int cancelOrder(bool lastCancel = false);  

  double halfSpread;
  double placePrice;
  int leftQty;
  int placeQty;
  int numOrder;
  int numCancel;
  OrderState state;
  OrderLeg activeLeg;

  std::string outOrderId;
  std::string respAddr;
  std::chrono::system_clock::time_point arrivalOrderTs;
  std::chrono::system_clock::time_point lastLegTs;
  double lastLegMid;
  OrderLeg lastLeg;
};
#endif
