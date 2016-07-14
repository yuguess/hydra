#include <iostream>
#include "ManualOrder.h"
#include <unistd.h>

ManualOrder::ManualOrder() {
  ProtoBufHelper::setupProtoBufMsgHub(msgHub);
  msgHub.registerCallback(std::bind(&ManualOrder::onUpdateMsg, this,
        std::placeholders::_1));

  CedarJsonConfig::getInstance().getStringByPath("SendToAddress", sendAddr);
  CedarJsonConfig::getInstance().getStringByPath("PublisherAddress", pubAddr);
}

int ManualOrder::onMsg(MessageBase msg) {
  CedarMsgType type = msg.type();
  ResponseMessage rmsg = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);

  std::cout << "type:" << rmsg.type() << std::endl;


  std::cout << "id:" << rmsg.id() << std::endl;
  std::cout << "error code:" << rmsg.error_code() << std::endl;
  LOG(INFO) << "id:" << rmsg.id();
}


int ManualOrder::onMsgTest(MessageBase msg) {
  CedarMsgType type = msg.type();
  ResponseMessage rmsg = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);

  LOG(INFO) << "id" << rmsg.id();
}

int ManualOrder::onUpdateMsg(MessageBase msg) {
  std::cout<< "*" << std::endl;
  CedarMsgType type= msg.type();
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

  std::vector<std::string> names, srvAddrs, bcstAddrs;
  CedarJsonConfig::getInstance().getStringArrayWithTag(names, "DataServer",
      "name");
  CedarJsonConfig::getInstance().getStringArrayWithTag(srvAddrs, "DataServer",
      "serverAddr");
  CedarJsonConfig::getInstance().getStringArrayWithTag(bcstAddrs, "DataServer",
      "boardcastAddr");
  for (int i = 0; i < names.size(); i++) {
    dataServers.push_back({names[i], srvAddrs[i], bcstAddrs[i]});
  }

  std::vector<std::string> tnames, addrs;
  CedarJsonConfig::getInstance().getStringArrayWithTag(tnames, "TradeServer",
      "name");
  CedarJsonConfig::getInstance().getStringArrayWithTag(addrs, "TradeServer",
      "address");
  for (int i = 0; i < names.size(); i++) {
    TradeServer tmp = {tnames[i], addrs[i]};
    tradeServers.push_back(tmp);
  }
}

void ManualOrder::run() {
  while (true) {
    char value;
    std::cout << std::endl
      << "1) Enter OrderRequest NewOrder/FirstLevel/SmartOrder" << std::endl
      << "2) Enter DataRequest" << std::endl;
    std::cin >> value;
    switch (value) {
      case '1':
        queryEnterOrder();
        break;
      case '2':
        queryDataRequest();
        break;

      default: throw std::exception();
    }
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
  for (int i = 0; i < dataServers.size(); i++)
    std::cout << i <<") Enter dataServer " << dataServers[i].name << std::endl;

  std::cin >> value;
  std::string sendAddr = dataServers[value].serverAddr;
  std::string boardcastAddr = dataServers[value].boardcastAddr;

  LOG(INFO) << "send msg to " << sendAddr;
  LOG(INFO) << req.DebugString();
  msgHub.pushMsg(sendAddr, ProtoBufHelper::wrapMsg(TYPE_DATAREQUEST, req));

  std::string chan = code + "." + CedarHelper::exchangeTypeToString(xchg);
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


int ManualOrder::queryCancelOrder() {
  std::cout << "\nNewOrder Request\n";
  static std::string respAddr = CedarHelper::getResponseAddr();

  OrderRequest order;
  order.set_response_address(respAddr);

  //order.set_id(queryID());
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
  << "1) Enter NewOrder/FirstLevel/SmartOrder Request" << std::endl
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

std::string ManualOrder::queryAccount() {

  char value;
  std::cout << "support options below:" << std::endl
            << "1) 2001_Stock" << std::endl
            << "2) 2001_Futures" << std::endl;
  std::cin >> value;
  switch (value) {
    case '1': return "2001_Stock";
    case '2': return "2001_Futures";
    default: throw std::exception();
  }
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
  << "4) SmartOrder" << std::endl
  << "5) FirstLevel" << std::endl
  << "OrdType: ";

  std::cin >> value;
  switch (value) {
    case '1': return RequestType::TYPE_LIMIT_ORDER_REQUEST;
    case '2': return RequestType::TYPE_MARKET_ORDER_REQUEST;
    case '3': return RequestType::TYPE_CANCEL_ORDER_REQUEST;
    case '4': return RequestType::TYPE_SMART_ORDER_REQUEST;
    case '5': return RequestType::TYPE_FIRST_LEVEL_ORDER_REQUEST;
    default: throw std::exception();
  }
}

double ManualOrder::queryPrice() {
  double value;
  std::cout << std::endl << "Price (only valid for certain order like limit): ";
  std::cin >> value;
  return value;
}

int ManualOrder::queryEnterOrder() {
  std::cout << "Send Msg Type\n";
  static std::string respAddr = CedarHelper::getResponseAddr();

  OrderRequest order;
  order.set_response_address(respAddr);
  order.set_id(CedarHelper::getOrderId());

  order.set_type(queryOrdType());
  order.set_limit_price(queryPrice());
  order.set_open_close(queryOrdPosition());
  order.set_exchange(SZSE);

  int value;
  std::cout << std::endl;
  for (int i = 0; i < dataServers.size(); i++) {
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

    case TYPE_SMART_ORDER_REQUEST:
      break;

    case TYPE_FIRST_LEVEL_ORDER_REQUEST:
      order.set_code(queryCode());
      order.set_exchange(queryExchange());
      order.set_buy_sell(querySide());
      order.set_trade_quantity(queryOrderQty());
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

