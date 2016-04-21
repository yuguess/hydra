#ifndef ORDER_DELEGATE_H
#define ORDER_DELEGATE_H

#include "StratBase.h"
#include "CedarHelper.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"

class OrderDelegate {

public:
  OrderDelegate(std::shared_ptr<StratBase> ptr) : pStratBase(ptr) {}
  int sendRequest(OrderRequest&);
  int onTickUpdate(MarketUpdate&);
  int onOrderResponseUpdate(ResponseMessage&);
  std::string getOrderIdString() {
    static int id = 0;
    const static int digit = 6;
    id++;
    std::string idStr = std::to_string(id);
    idStr = std::string(digit - idStr.length(), '0') + idStr;
    return idStr;
  }

private:
  std::map<std::string, std::vector<MarketUpdate>> dataBufs;
  std::shared_ptr<StratBase> pStratBase;
};

#endif
