#include "SmartOrder.h"
#include "CedarHelper.h"
#include "easylogging++.h"

int SmartOrder::onMsg(MessageBase &msg) {
  LOG(INFO) << "SmartOrder onMsg";
  switch (msg.type()) {
    case TYPE_MARKETUPDATE: {
      MarketUpdate mkt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
      onMktUpdate(mkt);
      break;
    }

    case TYPE_ORDER_REQUEST: {
      OrderRequest req = ProtoBufHelper::unwrapMsg<OrderRequest>(msg);
      onOrderRequest(req);
      break;
    }

    case TYPE_RESPONSE_MSG: {
      ResponseMessage rsp = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
      onResponseMsg(rsp);
      break;
    }

    default:
      LOG(WARNING) << "recv invalid msg type " << msg.type();
      LOG(INFO) << msg.DebugString();
  }
}

int SmartOrder::onMktUpdate(MarketUpdate&) {

  return 0;
}

int SmartOrder::onOrderRequest(OrderRequest&) {

  return 0;
}

int SmartOrder::onResponseMsg(ResponseMessage&) {
  return 0;
}

