#ifndef RANGE_STAT_H
#define RANGE_STAT_H

#include <boost/date_time/posix_time/posix_time.hpp>
#include "Backtester.h"

struct RangeStatResult {
  double open, high, low, close;
};

class RangeStat {
public:
  RangeStat(int rangeSeconds) {

  }

  //rng in seconds
  //int init(int rng) {
  //  rangePeriod = boost::posix_time::seconds(rng);
  //  resetRangeStat();
  //}

  int onTickUpdate(MarketUpdate &mkt) {
    //static boost::posix_time::ptime startingTime = Backtester::toTimestamp(mkt);

    //boost::posix_time::ptime curSt = Backtester::toTimestamp(mkt);
    //boost::posix_time::time_duration diff;
    //if (curSt >= startingTime)
    //  diff = curSt - startingTime;
    //else {
    //  LOG(ERROR) << "Recv 2 tick timestamp run backward !";
    //  return -1;
    //}

    //if (diff > rangePeriod) {
    //  RangeStatData rng({open, high, low, close});

    //  rngCallback(rng);

    //  resetRangeStat();
    //  open = mkt.last_price();
    //  close = mkt.last_price();
    //  high = std::max(high, mkt.last_price());
    //  low = std::min(low, mkt.last_price());
    //  startingTime = Backtester::toTimestamp(mkt);
    //} else {
    //  close = mkt.last_price();
    //  high = std::max(high, mkt.last_price());
    //  low = std::min(low, mkt.last_price());
    //}

    return 0;
  }

  int resetRangeStat() {
    open = 0;
    close = 0;
    high = 0;
    low = INT_MAX;
    return 0;
  }

private:
  boost::posix_time::time_duration rangePeriod;
  double open, close, high, low;
};

#endif
