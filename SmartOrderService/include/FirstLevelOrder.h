#ifndef FIRST_LEVEL_ORDER_H
#define FIRST_LEVEL_ORDER_H

#include "OrderReactor.h"
#include "ProtoBufMsgHub.h"
#include "SmartOrderService.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "CedarTimeHelper.h"

class FirstLevelOrder : public OrderReactor {

public:
  FirstLevelOrder(OrderRequest&, SmartOrderService *srcv);
  virtual int onMsg(MessageBase &);

private:
  const static int stockMinimumQty = 100;
  const static int volMulti = 2;
  const static int topVolMulti = 2;

  boost::posix_time::ptime getTimeThre() const {
    boost::posix_time::time_duration hms(14, 55, 30);
    boost::posix_time::ptime ptmDate(CedarTimeHelper::getCurPTime().date());
    return ptmDate + hms;
  }

  enum FirstLevelOrderState {
    Init = 0,
    Ready,
    Sending,
    Sent,
    Canceling,
    TerminateCancel,
    ChaseCanceling,
    ChaseReady
  };

  int cancelOrder();
  bool isInActiveMode(MarketUpdate&, TradeDirection);
  bool isBreakActiveMode(double , double , TradeDirection);
  bool sendLimitOrder(double);

  FirstLevelOrderState orderState;
  int leftQty;
  double placePrice, oneTick;
  std::string respAddr;
  std::string outOrderId;

  bool activeMode;
  boost::posix_time::ptime ptimeThre;
};

#endif
