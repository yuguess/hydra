#ifndef MARKETUPDATE_HELPER
#define MARKETUPDATE_HELPER

#include "CPlusPlusCode/ProtoBufMsg.pb.h"

class MarketUpdateHelper {

public:
  static int totalBidVol(MarketUpdate &mkt) {
    int bidVol = 0;
    for (int i = 0; i < mkt.bid_volume_size(); i++) {
      bidVol += mkt.bid_volume(i);
    }
    return bidVol;
  }

  static int totalAskVol(MarketUpdate &mkt) {
    int askVol = 0;
    for (int i = 0; i < mkt.ask_volume_size(); i++) {
      askVol += mkt.ask_volume(i);
    }
    return askVol;
  }
};

#endif

