#ifndef SMALL_ORDER_H
#define SMALL_ORDER_H

#include "OrderReactor.h"
#include "CedarTimeHelper.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "ProtoBufMsgHub.h"
#include "SmallOrder.h"

class SmallOrder : public OrderReactor {

public:
  SmallOrder(OrderRequest &, SmartOrderService*);
  int onMsg(MessageBase &);

private:
  //recalculate fair price every 7 mins
  const static int refreshTimePeriod = 7 * 60;
  const static int refreshTimeDelta = 70;
  const static int maxTimePeriod = 90 * 60;
  const static int stockMinimumQty = 100;
  const static int touchSizePercent = 80;
  const static int maxNumOrder = 100;
  const static int mmHSpreadMultiple = 2.5;
  const static int breakSessionPeriod = 90 * 60;

  //TODO: read out session boundary from market specification config
  boost::posix_time::ptime getMorningEnd() const {
    boost::posix_time::time_duration hms(11, 30, 0);
    boost::posix_time::ptime ptmDate(CedarTimeHelper::getCurPTime().date());

    return ptmDate + hms;
  }

  boost::posix_time::ptime getAfternoonStart() const {
    boost::posix_time::time_duration hms(13, 0, 0);
    boost::posix_time::ptime ptmDate(CedarTimeHelper::getCurPTime().date());

    return ptmDate + hms;
  }

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
  int lotSize;
  OrderState state;
  OrderLeg activeLeg;

  std::string outOrderId;
  std::string respAddr;
  boost::posix_time::ptime arrivalOrderPT;
  boost::posix_time::ptime lastLegPT;
  boost::posix_time::ptime morningEnd;
  boost::posix_time::ptime afternoonStart;
  double lastLegMid;
  OrderLeg lastLeg;
};
#endif
