#ifndef POSITION_MANAGER_H
#define POSITION_MANAGER_H

struct ItemPosition {
  ItemPosition():
    lastPrice(0.0), longPosition(0), shortPosition(0), netPosition(0) {}

  double updatePNL() {
    return pnl = (lastPrice - initPrice) * netPosition;
  }

  double lastPrice;
  double initPrice;
  int longPosition;
  int shortPosition;
  int netPosition;
  double pnl;
};

class PositionManager {

public:
  int onTickUpdate(MarketUpdate &mkt) {
    if (positionMap.find(mkt.code()) == positionMap.end()) {
      ItemPosition pos;
      pos.lastPrice = mkt.last_price();
      pos.initPrice = mkt.last_price();

      positionMap[mkt.code()] = pos;
    } else {
      positionMap[mkt.code()].lastPrice = mkt.last_price();
      positionMap[mkt.code()].updatePNL();
    }

    return 0;
  }

  int onOrderResponseUpdate(ResponseMessage &respMsg) {
    if (respMsg.type() != TYPE_TRADE)
      return -1;
    std::string ticker = respMsg.code();

    if (positionMap.find(ticker) == positionMap.end()) {
      ItemPosition pos;
      positionMap[ticker] = pos;
    }

    if (respMsg.trade_quantity() > 0)
      positionMap[ticker].longPosition = respMsg.trade_quantity();
    else if (respMsg.trade_quantity() < 0)
      positionMap[ticker].shortPosition =respMsg.trade_quantity();
    else {
      LOG(ERROR) << "Recv a response with 0 traded position";
    }
    ItemPosition pos = positionMap[ticker];
    positionMap[ticker].netPosition = pos.longPosition - pos.shortPosition;
    positionMap[ticker].updatePNL();

    return 0;
  }

  const std::map<std::string, ItemPosition>& getPosition() {
    return positionMap;
  }

private:
  std::map<std::string, ItemPosition> positionMap;
};

#endif
