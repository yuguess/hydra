#ifndef RANGE_STAT_H
#define RANGE_STAT_H

#include <algorithm>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Backtester.h"
#include "CedarTimeHelper.h"

namespace pt = boost::posix_time;

struct RangeStatResult {
  double open, high, low, close;
};

struct TimeInterval {
  pt::ptime start;
  pt::ptime end;
};

class RangeStat {
public:
  RangeStat(int rangeSeconds, std::string startTime, std::string endTime) :
    marketOpen(CedarTimeHelper::strToPTime("%H:%M:%S", startTime)),
    marketClose(CedarTimeHelper::strToPTime("%H:%M:%S", endTime)),
    rangePeriod(pt::seconds(rangeSeconds)) {

    curTimePeriod.start = marketOpen;
    curTimePeriod.end = marketOpen + rangePeriod;

    rangeStat.open = UNINITIALIZE;
    rangeStat.low = INT_MAX;
    rangeStat.high = INT_MIN;
  }

  bool onTickUpdate(MarketUpdate &mkt, RangeStatResult &res) {
    std::string tsStr = mkt.exchange_timestamp();
    pt::ptime ts = CedarTimeHelper::strToPTime("%H%M%S%F", tsStr);

    if (ts < marketOpen || ts > marketClose)
      return false;

    LOG(INFO) << "range start " << curTimePeriod.start;
    LOG(INFO) << "range end " << curTimePeriod.end;

    if (ts >= curTimePeriod.start && ts < curTimePeriod.end) {
      if (rangeStat.open == UNINITIALIZE)
        rangeStat.open = mkt.last_price();

      rangeStat.close = mkt.last_price();
      rangeStat.low = std::min(rangeStat.low, mkt.last_price());
      rangeStat.high = std::max(rangeStat.high, mkt.last_price());

      return false;
    } else if (ts >= curTimePeriod.end)  {

      do  {
        curTimePeriod.start += rangePeriod;
        curTimePeriod.end += rangePeriod;
        if (curTimePeriod.start >= marketClose) {
          curTimePeriod.start = marketOpen;
          curTimePeriod.end = marketOpen + rangePeriod;
        }
      } while (ts > curTimePeriod.end);

      res = rangeStat;

      rangeStat.open = mkt.last_price();
      rangeStat.high = mkt.last_price();
      rangeStat.low = mkt.last_price();
      rangeStat.close = mkt.last_price();

      return true;
    }

    return false;
  }

private:
  const static int UNINITIALIZE = -1;

  RangeStatResult rangeStat;
  TimeInterval curTimePeriod;
  pt::ptime marketOpen;
  pt::ptime marketClose;
  pt::time_duration rangePeriod;
};

#endif
