#ifndef ORDER_DELEGATE_H
#define ORDER_DELEGATE_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"

class OrderDelegate {
public:
  int onTickUpdate(MarketUpdate &);
  int sendRequest(OrderRequest &);

private:
  std::map<std::string, std::vector<MarketUpdate>> dataBufs; 
};

#endif
