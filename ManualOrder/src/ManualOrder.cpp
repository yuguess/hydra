#include <iostream>
#include "ManualOrder.h"
#include <unistd.h>
#include "BatTest.h"

ManualOrder::ManualOrder() {
  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(std::bind(&ManualOrder::onUpdateMsg, this,
        std::placeholders::_1));

  CedarJsonConfig::getInstance().getStringByPath("SendToAddress", sendAddr);
  CedarJsonConfig::getInstance().getStringByPath("PublisherAddress", pubAddr);
}

int ManualOrder::onMsg(MessageBase msg) {
  MsgType type = msg.type();
  ResponseMessage rmsg = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);

  std::cout << "type:" << rmsg.type() << std::endl;


  std::cout << "id:" << rmsg.id() << std::endl;
  std::cout << "error code:" << rmsg.error_code() << std::endl;
  LOG(INFO) << "id:" << rmsg.id();
}


int ManualOrder::onMsgTest(MessageBase msg) {
  MsgType type = msg.type();
  ResponseMessage rmsg = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);

  LOG(INFO) << "id" << rmsg.id();
}

int ManualOrder::onUpdateMsg(MessageBase msg) {
  std::cout<< "*" << std::endl;
  MsgType type= msg.type();
  MarketUpdate mUpdate = ProtoBufHelper::unwrapMsg<MarketUpdate>(msg);
  std::cout << "code:" <<mUpdate.code() << std::endl;
  std::cout << "exchange:" <<mUpdate.exchange() << std::endl;
  std::cout << "pre_close_price:" <<mUpdate.pre_close_price() << std::endl;
  std::cout << "open_price:" <<mUpdate.open_price() << std::endl;
  std::cout << "last price:" <<mUpdate.last_price() << std::endl;
  std::cout << "recv time:" <<mUpdate.recv_timestamp() << std::endl;
 
  std::cout << "bid_price 1:" <<mUpdate.bid_price(0) << std::endl; 
  std::cout << "bid_price 2:" <<mUpdate.bid_price(1) << std::endl;
  std::cout << "bid_price 3:" <<mUpdate.bid_price(2) << std::endl;
  std::cout << "bid_price 4:" <<mUpdate.bid_price(3) << std::endl;
  std::cout << "bid_price 5:" <<mUpdate.bid_price(4) << std::endl;


  std::cout << "bid_volume 1:" <<mUpdate.bid_volume(0) << std::endl; 
  std::cout << "bid_volume 2:" <<mUpdate.bid_volume(1) << std::endl;
  std::cout << "bid_volume 3:" <<mUpdate.bid_volume(2) << std::endl;
  std::cout << "bid_volume 4:" <<mUpdate.bid_volume(3) << std::endl;
  std::cout << "bid_volume 5:" <<mUpdate.bid_volume(4) << std::endl;


  std::cout << "ask_price 1:" <<mUpdate.ask_price(0) << std::endl; 
  std::cout << "ask_price 2:" <<mUpdate.ask_price(1) << std::endl;
  std::cout << "ask_price 3:" <<mUpdate.ask_price(2) << std::endl;
  std::cout << "ask_price 4:" <<mUpdate.ask_price(3) << std::endl;
  std::cout << "ask_price 5:" <<mUpdate.ask_price(4) << std::endl;

  std::cout << "ask_volume 1:" <<mUpdate.ask_volume(0) << std::endl; 
  std::cout << "ask_volume 2:" <<mUpdate.ask_volume(1) << std::endl;
  std::cout << "ask_volume 3:" <<mUpdate.ask_volume(2) << std::endl;
  std::cout << "ask_volume 4:" <<mUpdate.ask_volume(3) << std::endl;
  std::cout << "ask_volume 5:" <<mUpdate.ask_volume(4) << std::endl;

  std::cout << std::endl;

  LOG(INFO) << "[REALTIMETEST]id:" << mUpdate.code() << "|ask1 price:" << mUpdate.ask_price(0);
  LOG(INFO) << "[REALTIMETEST]id:" << mUpdate.code()  << "|ask1 volume:" << mUpdate.ask_volume(0);
  
  LOG(INFO) << "[REALTIMETEST]id:" << mUpdate.code()  << "|bid1 price:" << mUpdate.bid_price(0);
  LOG(INFO) << "[REALTIMETEST]id:" << mUpdate.code()  << "|bid1 volume:" << mUpdate.bid_volume(0);

  LOG(INFO) << "[REALTIMETEST]id:" << mUpdate.code()  << "|last price:" << mUpdate.last_price();

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

void ManualOrder::sendSingleOrder(string id, string code, TradeDirection buy_sell, int quantity) {
  OrderRequest order;
  order.set_response_address(CedarHelper::getResponseAddr());

  order.set_id(id);
  order.set_code(code);
  order.set_buy_sell(buy_sell);
  order.set_trade_quantity(quantity);
  order.set_limit_price(2.9);
  order.set_open_close(PositionDirection::OPEN_POSITION);
  order.set_exchange(SHSE);

  msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_ORDER_REQUEST, order));
}


void ManualOrder::cancelSingleOrder(string id, string ref_id) {
  OrderRequest order;
  order.set_response_address(CedarHelper::getResponseAddr());
  order.set_type(TYPE_CANCEL_ORDER_REQUEST);
  order.set_id(id);
  order.set_cancel_order_id(ref_id);


  msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_ORDER_REQUEST, order));
}

void ManualOrder::test1() {
  int count = 1;
  int i = 0;
  unsigned int mic=1;
  static std::string respAddr = CedarHelper::getResponseAddr();
  string tmp;

  for (i=0;i<20;i++) {  
    tmp = to_string(i);
    sendSingleOrder(tmp,"518880",TradeDirection::LONG_BUY,100);
  //  sleep(1);
  }

  for (i=0;i<20;i++) {  
    tmp = to_string(i);
  //  sleep(1);
    cancelSingleOrder("4897",tmp); 
  }
  
  
  getchar();
}



void ManualOrder::test2() {
  int count = 1;
  int i = 0;
  unsigned int mic=1;
  static std::string respAddr = CedarHelper::getResponseAddr();
  string tmp;

  for (i=0;i<50;i++) {  
    tmp = to_string(i);
    sendSingleOrder(tmp,"518880",TradeDirection::LONG_BUY,100);
  //  sleep(1);
    cancelSingleOrder("4897",tmp); 
  }
  getchar();
}

void ManualOrder::updataTest() {
  DataRequest dataReq;
  dataReq.set_code("000404");
  dataReq.set_exchange(SZSE);

  msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST,dataReq));

  
  dataReq.set_code("000418");
  dataReq.set_exchange(SZSE);

  msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST,dataReq));
  
  dataReq.set_code("002340");
  dataReq.set_exchange(SZSE);

  msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST,dataReq));
  
  dataReq.set_code("000060");
  dataReq.set_exchange(SZSE);

 // msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST,dataReq));
  
  getchar();
}


void ManualOrder::updateTestBoardcast() {
  DataRequest dataReq;
  dataReq.set_code("000404");
  dataReq.set_exchange(SZSE);

  msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST,dataReq));

  msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST,dataReq));
  dataReq.set_code("000418");
  dataReq.set_exchange(SZSE);
  msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST,dataReq));
  
  dataReq.set_code("002340");
  dataReq.set_exchange(SZSE);
  msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST,dataReq));
  
  
  dataReq.set_code("000060");
  dataReq.set_exchange(SZSE);
  msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST,dataReq));
  

  msgHub.addSubscription(pubAddr, "000061.SZSE");
  msgHub.addSubscription(pubAddr, "002340.SZSE");
  msgHub.addSubscription(pubAddr, "000418.SZSE");
  msgHub.addSubscription(pubAddr, "000404.SZSE");
  std::cout<<"subscripe complete";

  getchar();
}

void ManualOrder::autorun() {
  //test1();
  updateTestBoardcast();
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


std::string ManualOrder::queryCancelID() {
  std::string ID;
  std::cout << "CancelID:" << std::endl;
  std::cin >> ID;
  return ID;
}


int ManualOrder::queryCancelOrder() {
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
  order.set_cancel_order_id(queryCancelID());

  msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_ORDER_REQUEST, order));

  return 0;
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
  order.set_exchange(SZSE);
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
