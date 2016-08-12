#ifndef SMART_ORDER_H
#define SMART_ORDER_H

#include <chrono>
#include "OrderReactor.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "ProtoBufMsgHub.h"
#include "SmartOrderService.h"

class SmartOrder : public OrderReactor {

public:
  SmartOrder(OrderRequest &, SmartOrderService*);
  int onMsg(MessageBase &);

private:
  //recalculate fair price every 30 seconds
  const static int refreshTimePeriod = 30;
  const static int stockMinimumQty = 100;
  enum SmartOrderState {
    Init = 0,
    Ready,
    Sending,
    Sent,
    Canceling,
    TerminateCancel
  };

  int onMktUpdate(MarketUpdate&);
  int onOrderRequest(OrderRequest&);
  int onResponseMsg(ResponseMessage&);

  double calReferencePrice(MarketUpdate &);
  double calWeightPrice(MarketUpdate &);
  double avgBidPrice(MarketUpdate&);
  double avgAskPrice(MarketUpdate&);
  int totalBidVol(MarketUpdate&);
  int totalAskVol(MarketUpdate&);
  bool isOnlyOneInTopLadder(int);
  bool sendNewLimitOrder(MarketUpdate&);
  int cancelOrder();
  bool isPriceExpire();
  double roundUpLadder(double);
  double roundDownLadder(double);
  bool isValidMkt(MarketUpdate&);

  SmartOrderState state;
  double referencePrice;
  int leftQty;
  std::string outOrderId;
  std::string respAddr;
  double oneTick;
  double lastOrderPrice;
  std::chrono::system_clock::time_point lastOrderTs;
};
#endif
