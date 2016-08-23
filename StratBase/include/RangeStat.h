#ifndef RANGE_STAT_H
#define RANGE_STAT_H

#include <algorithm>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Backtester.h"
#include "CedarTimeHelper.h"

namespace pt = boost::posix_time;

struct RangeStatResult {
  RangeStatResult():
    high(INT_MIN), low(INT_MAX), tickCount(0), state(Uninitialize) {}

  bool init(MarketUpdate &mkt) {
    open = mkt.last_price();
    high = mkt.last_price();
    low = mkt.last_price();
    close = mkt.last_price();
    tickCount = 1;

    state = RangeStatResult::Running;
    return true;
  }

  bool update(MarketUpdate &mkt) {
    if (state == CloseOnLast) {
      return init(mkt);
    }

    close = mkt.last_price();
    low = std::min(low, mkt.last_price());
    high = std::max(high, mkt.last_price());
    tickCount++;

    return true;
  }

  bool closeOnLast() {
    high = INT_MIN;
    low = INT_MAX;
    tickCount = 0;
    state = CloseOnLast;

    return true;
  }

  enum State {
    Uninitialize = 0,
    Running,
    CloseOnLast,
  };

  double open, high, low, close;
  int tickCount;
  State state;
  pt::ptime start;
  pt::ptime end;
};

struct TimeInterval {
  pt::ptime start;
  pt::ptime end;
};

class RangeStatCollector {
public:
  RangeStatCollector(int rangeSeconds, std::string mss, std::string mse,
    std::string afterStart, std::string afterEnd) :
    morningSessionStart(mss), morningSessionEnd(mse),
    afternoonSessionStart(afterStart), afternoonSessionEnd(afterEnd),
    rangePeriod(pt::seconds(rangeSeconds)) {
  }

  bool onTickUpdate(MarketUpdate &mkt, RangeStatResult &res) {
    std::string tmp;
    std::string tsStr = mkt.trading_day() + mkt.exchange_timestamp();
    pt::ptime ts = CedarTimeHelper::strToPTime("%Y%m%d%H%M%S%F", tsStr);

    tmp = mkt.trading_day() + afternoonSessionStart;
    afternoonStart = CedarTimeHelper::strToPTime("%Y%m%d%H:%M:%S", tmp);
    tmp = mkt.trading_day() + afternoonSessionEnd;
    afternoonEnd = CedarTimeHelper::strToPTime("%Y%m%d%H:%M:%S", tmp);
    tmp = mkt.trading_day() + morningSessionEnd;
    morningEnd = CedarTimeHelper::strToPTime("%Y%m%d%H:%M:%S", tmp);

    //LOG(INFO) << "curTimePeriod start " << curTimePeriod.start;
    //LOG(INFO) << "curTimePeriod end " << curTimePeriod.end;
    //LOG(INFO) << "ts " << ts;

    if (rangeStat.state == RangeStatResult::Uninitialize) {
      rangeStat.init(mkt);
      setNewMarketDay(mkt.trading_day());
    }

    while (ts > curTimePeriod.end) {
      if (tradingDay != mkt.trading_day()) {
        setNewMarketDay(mkt.trading_day());
      } else {
        curTimePeriod.start += rangePeriod;
        curTimePeriod.end += rangePeriod;

        if (curTimePeriod.start == morningEnd) {
          //setToAfternoonSession();
          curTimePeriod.start = afternoonStart;
          curTimePeriod.end = curTimePeriod.start + rangePeriod;
        } else if (curTimePeriod.start == afternoonEnd) {
          //skip to next day
          setNewMarketDay(addOneDay(mkt.trading_day()));
        }
      }

      if (ts >= curTimePeriod.start && ts <= curTimePeriod.end) {
        res = rangeStat;
        rangeStat.init(mkt);
        LOG(INFO) << "inRange";
        return true;
      }
    }

    if (ts < curTimePeriod.start)
      return false;

    if (curTimePeriod.start <= ts && ts <=  curTimePeriod.end) {
      rangeStat.start = curTimePeriod.start;
      rangeStat.end = curTimePeriod.end;
      rangeStat.update(mkt);
    }

    return false;
  }

private:
  bool setNewMarketDay(std::string dayStr) {
    std::string tsStr = dayStr + morningSessionStart;
    curTimePeriod.start = CedarTimeHelper::strToPTime(
        "%Y%m%d%H:%M:%S", tsStr);
    curTimePeriod.end = curTimePeriod.start + rangePeriod;
    tradingDay = dayStr;

    return true;
  }

  std::string addOneDay(std::string dayStr) {
    pt::ptime ptm = CedarTimeHelper::strToPTime("%Y%m%d", dayStr);
    ptm += pt::hours(24);
    return CedarTimeHelper::ptimeToStr("%Y%m%d", ptm);
  }

  RangeStatResult rangeStat;
  TimeInterval curTimePeriod;

  std::string tradingDay;
  std::string morningSessionStart, morningSessionEnd;
  std::string afternoonSessionStart, afternoonSessionEnd;

  pt::ptime morningEnd, afternoonStart, afternoonEnd;
  pt::time_duration rangePeriod;
};

#endif
