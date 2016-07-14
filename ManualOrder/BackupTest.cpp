

int onMsgTest(MessageBase msg) {
  CedarMsgType type = msg.type();
  ResponseMessage rmsg = ProtoBufHelper::unwrapMsg<ResponseMessage>(msg);
  LOG(INFO) << "id" << rmsg.id();
}

void sendSingleOrder(string id, string code, TradeDirection buy_sell, int quantity) {
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
