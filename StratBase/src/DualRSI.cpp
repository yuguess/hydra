#include "DualRSI.h"

int DualRSI::onMsg(MessageBase &msg) {

  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mktUpdt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    //orderDelegate.onTickUpdate(mktUpdt);
    //twoMin.onTickUpdate(mktUpdt);
    //positionManager.onTickUpdate(mktUpdt);

  } else if (msg.type() == TYPE_RESPONSE_MSG) {
    ResponseMessage rsp = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
    ////update orderDelegate
    //orderDelegate.onOrderResponseUpdate(respMsg);
    //positionManager.onOrderResponseUpdate(respMsg);
  }

  return 0;
}

bool DualRSI::saveState(Json::Value &jsonState) {
  return true;
}

bool DualRSI::restoreState(Json::Value &jsonState) {
  return true;
}
