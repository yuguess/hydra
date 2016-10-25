#include "SmartOrderService.h"
#include "SmartOrder.h"
#include "SmallOrder.h"
#include "FirstLevelOrder.h"
#include "EnumStringMap.h"
#include "LogHelper.h"

SmartOrderService::SmartOrderService() {
  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(std::bind(&SmartOrderService::onMsg,
        this, std::placeholders::_1));

  CedarJsonConfig::getInstance().getStringByPath(
    "StockDataServer.serverAddr", stkDataAddr);
  CedarJsonConfig::getInstance().getStringByPath(
    "StockDataServer.boardcastAddr", stkBoardcastAddr);
  CedarJsonConfig::getInstance().getStringByPath(
    "FuturesDataServer.serverAddr", futuresDataAddr);
  CedarJsonConfig::getInstance().getStringByPath(
    "FuturesDataServer.boardcastAddr", futuresBoardcastAddr);

  CedarHelper::setupTradeServerMap(accToAddr);
}

int SmartOrderService::onMktUpdate(MarketUpdate &mkt, MessageBase &msg) {
  std::string chan = mkt.code() + "." + EnumToString::toString(mkt.exchange());

  if (mktDriver.find(chan) == mktDriver.end()) {
    LOG(INFO) << "invalid " << chan << " for mktDriver";
    return -1;
  }

  auto it = mktDriver[chan].begin();
  while (it != mktDriver[chan].end()) {
    if ((*it)->isRecycle()) {
      LOG(INFO) << "detect recycle Reactor order "
                  << (*it)->getOrderReactorID() << ", delete it";
      it = mktDriver[chan].erase(it);
    } else {
      //LOG(INFO) << "chan " << chan << " Req " << (*it)->getOrderReactorID();
      (*it)->onMsg(msg);
      it++;
    }
  }

  return 0;
}

int SmartOrderService::onOrderRequest(OrderRequest &req) {
  LOG(INFO) << "recv order request" << std::endl << req.DebugString();
  req.set_alg_order_id(CedarHelper::getOrderId());

  std::shared_ptr<OrderReactor> reactor;
  if (req.type() == TYPE_SMART_ORDER_REQUEST) {
    reactor = std::shared_ptr<OrderReactor>(new SmartOrder(req, this));
  } else if (req.type() == TYPE_SMALL_ORDER_REQUEST) {
    reactor = std::shared_ptr<OrderReactor>(new SmallOrder(req, this));
  } else if (req.type() == TYPE_FIRST_LEVEL_ORDER_REQUEST) {
    reactor = std::shared_ptr<OrderReactor>(new FirstLevelOrder(req, this));
  } else {
    LOG(ERROR) << "SmartOrderSrc rcv invalid OrderReq type " << req.type();
    return -1;
  }

  if (orderReactorIDtoObj.find(reactor->getOrderReactorID()) !=
      orderReactorIDtoObj.end()) {
    LOG(WARNING) << "Detect duplicate orderReactorID";
  }

  orderReactorIDtoObj[reactor->getOrderReactorID()] = reactor;
  std::string key = req.code() + "." + EnumToString::toString(req.exchange());
  addReactorToMktDriver(key, reactor);
  subscribeTicker(req.code(), req.exchange());

  LogHelper::logObject(req);

  return 0;
}

int SmartOrderService::onResponseMsg(ResponseMessage &rspMsg, MessageBase &msg) {
  if (rspMsg.type() == TYPE_CANCEL_ORDER_CONFIRM) {
    LOG(INFO) << "cancel confirm";
    LOG(INFO) << rspMsg.DebugString();

    if (rspDriver.find(rspMsg.ref_id()) == rspDriver.end()) {
      LOG(WARNING) << "Cancel confirm get id without rspDriver";
      return -1;
    }

    rspDriver[rspMsg.ref_id()]->onMsg(msg);

    std::string inID = rspDriver[rspMsg.ref_id()]->getOrderReactorID();
    if (rspDriver[rspMsg.ref_id()]->isRecycle()) {
      if (orderReactorIDtoObj.find(inID) == orderReactorIDtoObj.end()) {
        LOG(WARNING) << "Can not recycle a confirm orderReactor obj";
        return -1;
      }
      orderReactorIDtoObj.erase(inID);
    }
    rspDriver.erase(rspMsg.ref_id());

  } else if (rspMsg.type() == TYPE_TRADE) {

    LOG(INFO) << "trade";
    LOG(INFO) << rspMsg.DebugString();

    if (rspDriver.find(rspMsg.ref_id()) == rspDriver.end()) {
      LOG(WARNING) << "Trade get id without rspDriver";
      return -1;
    }
    rspDriver[rspMsg.ref_id()]->onMsg(msg);

    if (rspDriver[rspMsg.ref_id()]->isRecycle()) {
      rspDriver.erase(rspMsg.ref_id());
      LOG(INFO) << "all trade, rm frm rspDriver with id " << rspMsg.ref_id();
      orderReactorIDtoObj.erase(rspMsg.ref_id());
      LOG(INFO) << "Alltrade, rm frm orderIdToObj map wiz id "
        << rspMsg.ref_id();
    }

  } else if (rspMsg.type() == TYPE_ERROR) {
    LOG(INFO) << "error msg";
    LOG(INFO) << rspMsg.DebugString();

    if (rspDriver.find(rspMsg.ref_id()) == rspDriver.end())
      LOG(WARNING) << "TypeError Trade get id without rspDriver";

  } else if (rspMsg.type() == TYPE_NEW_ORDER_CONFIRM) {
    LOG(INFO) << "new order confirm";
    LOG(INFO) << rspMsg.DebugString();

    if (rspDriver.find(rspMsg.ref_id()) == rspDriver.end()) {
      LOG(WARNING) << "New Order Confirm get id without rspDriver";
      return -1;
    }

    rspDriver[rspMsg.ref_id()]->onMsg(msg);
  }

  return 0;
}

int SmartOrderService::onMsg(MessageBase msg) {
  switch (msg.type()) {
    case TYPE_MARKETUPDATE: {
      MarketUpdate mkt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
      onMktUpdate(mkt, msg);
      break;
    }

    case TYPE_ORDER_REQUEST: {
      OrderRequest req = ProtoBufHelper::unwrapMsg<OrderRequest>(msg);
      onOrderRequest(req);
      break;
    }

    case TYPE_RESPONSE_MSG: {
      ResponseMessage rsp = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
      onResponseMsg(rsp, msg);
      break;
    }

    default:
      LOG(WARNING) << "recv invalid msg type " << msg.type();
      LOG(INFO) << msg.DebugString();
  }

  return 0;
}

int SmartOrderService::addReactorToMktDriver(std::string &k,
    std::shared_ptr<OrderReactor> &order) {
  std::string targetID = order->getOrderReactorID();

  if (mktDriver.find(k) == mktDriver.end()){
    mktDriver[k] = std::list<std::shared_ptr<OrderReactor>>();
  } else {
    //ensure its uniq
    for (auto it = mktDriver[k].begin(); it !=  mktDriver[k].end(); it++) {
      if (targetID == (*it)->getOrderReactorID()) {
        LOG(WARNING) << "add duplicate id to mktDriver ";
        return -1;
      }
    }
  }

  mktDriver[k].push_back(order);
  return 0;
}

int SmartOrderService::subscribeTicker(std::string code, ExchangeType xchg) {
  DataRequest mdReq;
  mdReq.set_code(code);
  mdReq.set_exchange(xchg);

  std::string targetAddr;
  std::string targetBoardcastAddr;

  if (xchg == SHSE || xchg== SZSE) {
    targetAddr = stkDataAddr;
    targetBoardcastAddr = stkBoardcastAddr;
  } else {
    targetAddr = futuresDataAddr;
    targetBoardcastAddr = futuresBoardcastAddr;
  }

  LOG(INFO) << "send data request " << targetAddr;
  LOG(INFO) << mdReq.DebugString();

  msgHub.pushMsg(targetAddr,
    ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST, mdReq));
  std::string chan = code + "." + EnumToString::toString(xchg);

  msgHub.addSubscription(targetBoardcastAddr, chan);

  return 0;
}

int SmartOrderService::sendRequest(
    std::string orderReactorId, OrderRequest &req) {

  if (accToAddr.find(req.account()) == accToAddr.end()) {
    LOG(ERROR) << "Invalid account " << req.account();
    return -1;
  }

  std::string addr = accToAddr[req.account()];
  msgHub.pushMsg(addr, ProtoBufHelper::wrapMsg(TYPE_ORDER_REQUEST, req));

  if (req.type() == TYPE_LIMIT_ORDER_REQUEST) {
    LOG(INFO) << "add " << req.id() << " to rspDriver";
    rspDriver[req.id()] = orderReactorIDtoObj[orderReactorId];
  }

  LOG(INFO) << "orderService out request " << addr;
  LOG(INFO) << req.DebugString();
  return 0;
}
