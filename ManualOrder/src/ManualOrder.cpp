#include <iostream>
#include "ManualOrder.h"
#include <unistd.h>
#include "EnumStringMap.h"

ManualOrder::ManualOrder() {
  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(std::bind(&ManualOrder::onMsg, this,
        std::placeholders::_1));

  std::vector<std::string> names, srvAddrs, bcstAddrs;
  CedarJsonConfig::getInstance().getStringArrayWithTag(names, "DataServer",
      "name");
  CedarJsonConfig::getInstance().getStringArrayWithTag(srvAddrs, "DataServer",
      "serverAddr");
  CedarJsonConfig::getInstance().getStringArrayWithTag(bcstAddrs, "DataServer",
      "boardcastAddr");
  for (unsigned int i = 0; i < names.size(); i++) {
    dataServers.push_back({names[i], srvAddrs[i], bcstAddrs[i]});
  }

  std::vector<std::string> tnames, addrs;
  CedarJsonConfig::getInstance().getStringArrayWithTag(tnames, "TradeServer",
      "name");
  CedarJsonConfig::getInstance().getStringArrayWithTag(addrs, "TradeServer",
      "address");
  for (unsigned i = 0; i < tnames.size(); i++) {
    TradeServer tmp = {tnames[i], addrs[i]};
    tradeServers.push_back(tmp);

    serverAddrMap[tnames[i]] = addrs[i];

    LOG(INFO) << "TradeServer name:" << tnames[i] << "," << addrs[i];
  }
}

int ManualOrder::onMsg(MessageBase msg) {
  if (msg.type() == TYPE_MARKETUPDATE) {
    MarketUpdate mkt = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
    LOG(INFO) << mkt.DebugString();
  } if (msg.type() == TYPE_ORDER_REQUEST) {
    OrderRequest req = ProtoBufHelper::unwrapMsg<OrderRequest>(msg);
    LOG(INFO) << req.DebugString();
  } if (msg.type() == TYPE_RESPONSE_MSG) {
    ResponseMessage rmsg = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
    LOG(INFO) << rmsg.DebugString();
  }

  return 0;
}

void ManualOrder::run() {
  while (true) {
    char value;
    std::cout << std::endl
      << "1) Enter OrderRequest Limit/Market/Cancel" << std::endl
      << "2) Enter OrderRequest FirstLevel/SmartOrder/SmallOrder" << std::endl
      << "3) Enter DataRequest" << std::endl;
    std::cin >> value;
    switch (value) {
      case '1':
        queryEnterOrder();
        break;
      case '2':
        queryAlgOrder();
        break;
      case '3':
        queryDataRequest();
        break;

      default: throw std::exception();
    }
  }
}

int ManualOrder::queryDataRequest() {
  std::cout << "Send DataRequest\n";
  static std::string respAddr = CedarHelper::getResponseAddr();

  DataRequest req;
  std::string code = queryCode();
  ExchangeType xchg = queryExchange();
  req.set_code(code);
  req.set_exchange(xchg);

  int value;
  std::cout << std::endl;
  for (unsigned i = 0; i < dataServers.size(); i++)
    std::cout << i <<" for dataServer " << dataServers[i].name << std::endl;

  std::cin >> value;
  std::string sendAddr = dataServers[value].serverAddr;
  std::string boardcastAddr = dataServers[value].boardcastAddr;

  LOG(INFO) << "send msg to " << sendAddr;
  LOG(INFO) << req.DebugString();
  msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST, req));

  std::string chan = code + "." + EnumToString::toString(xchg);
  LOG(INFO) << "subscripe " << boardcastAddr;
  LOG(INFO) << "subscripe channel " << chan;
  msgHub.addSubscription(boardcastAddr, chan);

  return 0;
}


std::string ManualOrder::queryCancelID() {
  std::string ID;
  std::cout << "CancelID:" << std::endl;
  std::cin >> ID;
  return ID;
}

std::string ManualOrder::queryCode() {
  std::string value;
  std::cout << std::endl << "Code (stock 000001, futures jd1609): ";
  std::cin >> value;
  return value;
}

std::string ManualOrder::queryRefID() {
  std::string value;
  std::cout << std::endl << "refId: ";
  std::cin >> value;
  return value;
}

TradeDirection ManualOrder::querySide() {
  char value;
  std::cout << std::endl
  << "1) Buy" << std::endl
  << "2) Sell" << std::endl
  << "Side: ";

  std::cin >> value;
  switch (value) {
    case '1': return TradeDirection::LONG_BUY;
    case '2': return TradeDirection::SHORT_SELL;
    default: throw std::exception();
  }
}

int ManualOrder::queryOrderQty() {
  int qty;
  std::cout << std::endl << "OrderQty: ";
  std::cin >> qty;
  return qty;
}

ExchangeType ManualOrder::queryExchange() {

  char value;
  std::cout << "Support exchanges below:" << std::endl
            << "1) SHSE" << std::endl
            << "2) SZSE" << std::endl
            << "3) CFE" << std::endl
            << "4) SHFE" << std::endl
            << "5) DCE" << std::endl
            << "6) ZCE" << std::endl;

  std::cin >> value;
  switch (value) {
    case '1': return SHSE;
    case '2': return SZSE;
    case '3': return CFE;
    case '4': return SHFE;
    case '5': return DCE;
    case '6': return ZCE;
    default: throw std::exception();
  }
}

RequestType ManualOrder::queryOrdType() {
  char value;
  std::cout << std::endl
  << "1) Limit" << std::endl
  << "2) Market" << std::endl
  << "3) Cancel" << std::endl
  << "OrdType: ";

  std::cin >> value;
  switch (value) {
    case '1': return RequestType::TYPE_LIMIT_ORDER_REQUEST;
    case '2': return RequestType::TYPE_MARKET_ORDER_REQUEST;
    case '3': return RequestType::TYPE_CANCEL_ORDER_REQUEST;
    default: throw std::exception();
  }
}

RequestType ManualOrder::queryAlgOrdType() {
  char value;
  std::cout << std::endl
  << "1) FirstLevel" << std::endl
  << "2) SmartOrder" << std::endl
  << "3) SmallOrder" << std::endl;

  std::cin >> value;
  switch (value) {
    case '1': return RequestType::TYPE_FIRST_LEVEL_ORDER_REQUEST;
    case '2': return RequestType::TYPE_SMART_ORDER_REQUEST;
    case '3': return RequestType::TYPE_SMALL_ORDER_REQUEST;
    default: throw std::exception();
  }
}

double ManualOrder::queryPrice() {
  double value;
  std::cout << std::endl <<
      "Price (only valid for certain order like limit): ";
  std::cin >> value;
  return value;
}

int ManualOrder::queryAlgOrder() {
  static std::string respAddr = CedarHelper::getResponseAddr();

  OrderRequest order;
  order.set_response_address(respAddr);
  order.set_id(CedarHelper::getOrderId());
  order.set_alg_order_id(CedarHelper::getOrderId());
  order.set_batch_id(CedarHelper::getOrderId());
  order.set_type(queryAlgOrdType());

  order.set_code(queryCode());
  order.set_exchange(queryExchange());
  order.set_buy_sell(querySide());
  order.set_trade_quantity(queryOrderQty());

  setOrderReqAccount(order);

  msgHub.pushMsg(serverAddrMap["SmartOrderService"],
      ProtoBufHelper::wrapMsg(TYPE_ORDER_REQUEST, order));
  return 0;
}

int ManualOrder::setOrderReqAccount(OrderRequest &order) {
  int value;
  std::cout << std::endl;
  for (unsigned i = 0; i < tradeServers.size(); i++) {
    if (tradeServers[i].name.find("Stock") == std::string::npos)
      continue;
    std::cout << i <<") for " << tradeServers[i].name << std::endl;
  }
  std::cin >> value;

  if (value < 0 || value >= tradeServers.size()) {
    std::cout << "input index out of range" << std::endl;
    return -1;
  }
  order.set_account(tradeServers[value].name);

  return 0;
}

int ManualOrder::queryEnterOrder() {
  std::cout << "Send Msg Type\n";
  static std::string respAddr = CedarHelper::getResponseAddr();

  OrderRequest order;
  order.set_response_address(respAddr);
  order.set_id(CedarHelper::getOrderId());
  order.set_type(queryOrdType());

  int value;
  std::cout << std::endl;
  for (unsigned i = 0; i < tradeServers.size(); i++) {
    std::cout << i <<") for "
      << tradeServers[i].name << std::endl;
  }
  std::cin >> value;
  if (value < 0 || value >= tradeServers.size()) {
    std::cout << "input index out of range" << std::endl;
    return -1;
  }
  std::string sendAddr = tradeServers[value].address;

  switch (order.type()) {
    case TYPE_LIMIT_ORDER_REQUEST:
      order.set_code(queryCode());
      order.set_exchange(queryExchange());
      order.set_buy_sell(querySide());
      order.set_trade_quantity(queryOrderQty());
      order.set_limit_price(queryPrice());
      order.set_open_close(queryOrdPosition());
      break;

    case TYPE_MARKET_ORDER_REQUEST:
      break;

    case TYPE_CANCEL_ORDER_REQUEST:
      order.set_cancel_order_id(queryCancelID());
      break;

    default:
      break;
  }

  LOG(INFO) << "send msg to " << sendAddr;
  LOG(INFO) << order.DebugString();
  msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_ORDER_REQUEST, order));

  return 0;
}

PositionDirection ManualOrder::queryOrdPosition() {
  int value;
  std::cout << std::endl
  << "Only valid for futures order request" << std::endl
  << "1) Open" << std::endl
  << "2) Close" << std::endl
  << "3) CloseToday (only for SH futures)" << std::endl
  << "4) CloseYesterday (only for SH futures)" << std::endl
  << "OrdAction: ";

  std::cin >> value;
  switch (value) {
    case 1: return PositionDirection::OPEN_POSITION;
    case 2: return PositionDirection::CLOSE_POSITION;
    case 3: return PositionDirection::CLOSE_TODAY_POSITION;
    case 4: return PositionDirection::CLOSE_YESTERDAY_POSITION;
    default: throw std::exception();
  }
}
