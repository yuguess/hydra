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
  int cancelOrder();
  double avgBidPrice(MarketUpdate&);
  double avgAskPrice(MarketUpdate&);
  int totalBidVol(MarketUpdate&);
  int totalAskVol(MarketUpdate&);
  double roundToLadder(double, MarketUpdate&);
  double roundDownAskLadder(double, MarketUpdate&);
  double roundUpBidLadder(double, MarketUpdate&);
  double oneTickPrice(std::string&);
  bool isOnlyOneInTopLadder(int);
  bool sendNewLimitOrder(MarketUpdate&);
  bool isPriceExpire();

  SmartOrderState state;
  double referencePrice;
  int leftQty;
  std::string outOrderId;
  std::string respAddr;

  double lastOrderPrice;
  std::chrono::system_clock::time_point lastOrderTs;
};

#endif
