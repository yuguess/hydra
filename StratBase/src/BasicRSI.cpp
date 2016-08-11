#include <iostream>
#include "CedarHelper.h"
#include "ProtoBufMsgHub.h"
#include "BasicRSI.h"

int BasicRSI::onMsg(MessageBase &msg) {
  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mktUpdt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);

    //orderDelegate.onTickUpdate(mktUpdt);
    //twoMin.onTickUpdate(mktUpdt);
    //positionManager.onTickUpdate(mktUpdt);

  } else if (msg.type() == TYPE_RESPONSE_MSG) {
    //ResponseMessage respMsg = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
    ////update orderDelegate
    //orderDelegate.onOrderResponseUpdate(respMsg);
    //positionManager.onOrderResponseUpdate(respMsg);
  }

  //check stop profit/loss on every tick
  return 0;
}
