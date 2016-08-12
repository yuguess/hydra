#ifndef MARKET_SPEC_HELPER
#define MARKET_SPEC_HELPER

#include "CedarHelper.h"

class MarketSpecHelper {
public:
  static double getOneTick(std::string code) {
    if (CedarHelper::isStock(code)) {
      return getStockOneTick(code);
    }

    return getFuturesOneTick(code);
  }

private:
  static double getStockOneTick(std::string &code) {
    //SH,SZ stock has 6,3,0
    int firstCode = code[0] - '0';
    switch (firstCode) {
      case 0:
      case 3:
      case 6:
        return 0.01;
      case 1:
      case 5:
        return 0.001;
    }

    LOG(ERROR) << "invalid code " << code << " try to get oneTick";
    return 0.0;
  }

  static double getFuturesOneTick(std::string &code) {
    return 0.0;
  }
};

#endif
