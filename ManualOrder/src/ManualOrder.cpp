#include <iostream>
#include "ManualOrder.h"

ManualOrder::ManualOrder() {
  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(std::bind(&ManualOrder::onMsg, this,
        std::placeholders::_1));

  CedarJsonConfig::getInstance().getStringByPath("SendToAddress", sendAddr);
}

int ManualOrder::onMsg(MessageBase msg) {
  LOG(INFO) << "MessageBase:" << msg.type();
}

void ManualOrder::run() {
  while (true) {
    char action = queryAction();
    if (action == '1')
      queryEnterOrder();
    else if (action == '2')
      queryCancelOrder();
    else if (action == '3')
      break;
  }
}

//OrderAction ManualOrder::queryOrdAction(){
//  char value;
//  std::cout<<std::endl
//  << "1) NEW" << std::endl
//  << "2) CANCEL" << std::endl
//  << "OrderAction: ";
//
//  std::cin >> value;
//  switch (value) {
//    case '1': return NEW;
//    case '2': return CANCEL;
//  }
//}


std::string ManualOrder::queryChan(){
  char value;
  std::cout << std::endl
  << "1) FemasTradeRequest" << std::endl
  << "2) CTPTradeReturn" << std::endl
  << "3) FemasTradeReturn" << std::endl
  << "4) OrderHubReturn" << std::endl
  << "5) StatusMsg" << std::endl
  << "6) SymbolPosition" << std::endl
  << "7) GUICmdRequest" << std::endl
  << "8) ManualOrder" << std::endl
  << "Channel name: ";

  std::cin >> value;
  switch (value) {
    case '1': return "FemasTradeRequest" ;
    case '2': return "CTPTradeReturn";
    case '3': return "FemasTradeReturn";
    case '4': return "OrderHubReturn" ;
    case '5': return "OrderHubStatusMsg" ;
    case '6': return "SymbolPosition" ;
    case '7': return "GUICmdRequest" ;
    case '8': return "ManualOrder" ;
  }
}

//ReturnType ManualOrder::queryRtnType(){
//  char value;
//  std::cout << std::endl
//  << "1) CONFIRM" << std::endl
//  << "2) NEW_CONFIRM" << std::endl
//  << "3) CANCEL_CONFIRM" << std::endl
//  << "4) TRADE" << std::endl
//  << "5) ERROR" << std::endl
//  << "ReturnType: ";
//
//  std::cin >> value;
//  switch (value) {
//    case '1': return CONFIRM;
//    case '2': return NEW_CONFIRM;
//    case '3': return CANCEL_CONFIRM;
//    case '4': return CEDAR_TRADE;
//    case '5': return CEDAR_ERROR;
//  }
//}

int ManualOrder::queryCancelOrder() {
  printf("queryCancel order\n");
}

int ManualOrder::queryAction() {
  char value;
  std::cout << std::endl
  << "1) Enter New Order Request" << std::endl
  << "2) Enter Cancel Order Request (not support right now)" << std::endl
  << "3) Quit" << std::endl;
  std::cin >> value;
  switch (value) {
    case '1': return '1';
    case '2': return '2';
    case '3': return '3';
    default: throw std::exception();
  }

  return 0;
}

std::string ManualOrder::queryCode() {
  std::string value;
  std::cout << std::endl << "Code (format like 000001): ";
  std::cin >> value;
  return value;
}

std::string ManualOrder::queryID() {
  std::string value;
  std::cout << std::endl << "ID: ";
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

double ManualOrder::queryPrice() {
  double value;
  std::cout << std::endl << "Price (only valid for certain order like limit): ";
  std::cin >> value;
  return value;
}

RequestType ManualOrder::queryOrdType() {
  char value;
  std::cout << std::endl
  << "1) Market" << std::endl
  << "2) Limit" << std::endl
  << "OrdType: ";

  std::cin >> value;
  switch (value) {
    case '1': return RequestType::TYPE_LIMIT_ORDER_REQUEST;
    case '2': return RequestType::TYPE_MARKET_ORDER_REQUEST;
    default: throw std::exception();
  }
}

int ManualOrder::queryEnterOrder() {
  std::cout << "\nNewOrder Request\n";
  static std::string respAddr = CedarHelper::getResponseAddr();

  OrderRequest order;
  order.set_response_address(respAddr);

  order.set_id(queryID());
  order.set_code(queryCode());
  order.set_buy_sell(querySide());
  order.set_trade_quantity(queryOrderQty());
  order.set_type(queryOrdType());
  order.set_limit_price(queryPrice());
  order.set_open_close(queryOrdPosition());

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
