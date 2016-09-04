#ifndef RANGE_STAT_H
#define RANGE_STAT_H

#include <algorithm>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "Backtester.h"
#include "CedarTimeHelper.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "EnumStringMap.h"
#include "JsonHelper.h"

namespace pt = boost::posix_time;
namespace gg = boost::gregorian;

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
    if (state == Uninitialize) {
      return init(mkt);
    }

    close = mkt.last_price();
    low = std::min(low, mkt.last_price());
    high = std::max(high, mkt.last_price());
    tickCount++;

    return true;
  }

  enum State {
    Uninitialize = 0,
    Running
  };

  double open, high, low, close;
  int tickCount;
  State state;
  pt::ptime start;
  pt::ptime end;
};

class RangeCollector {
public:
  RangeCollector(int freq, std::string xch) : frequency(freq),
    rangePeriod(pt::seconds(freq)), endOffset(pt::seconds(offset)),
    periodIdx(0), updateTradingDay("") {

    exchange = StringToEnum::toExchangeType(xch);

    Json::Value root;
    JsonHelper::loadJsonFile(
      "../../ShareConfig/MarketSpecification.json", root);

    std::vector<std::string> starts, ends;
    JsonHelper::getStringArrayWithTag(root, "TradingSession." + xch,
      "start", starts);
    JsonHelper::getStringArrayWithTag(root, "TradingSession." + xch,
      "end", ends);

    for (unsigned i = 0; i < starts.size(); i++) {
      pt::ptime periodStart =
        CedarTimeHelper::strToPTime("%H:%M:%S", starts[i]);
      pt::ptime endTime = CedarTimeHelper::strToPTime("%H:%M:%S", ends[i]);
      endTime -= endOffset;

      while (periodStart < endTime) {
        periods.push_back(std::make_pair(periodStart,
          std::min(endTime, periodStart + rangePeriod)));
        periodStart += rangePeriod;
      }
    }

    for (unsigned i = 0; i < periods.size(); i++) {
      LOG(INFO) << periods[i].first << "---" << periods[i].second;
    }
  }

  bool onBacktestTickDataUpdate(MarketUpdate &mkt, RangeStatResult &res) {
    if (updateTradingDay != mkt.trading_day()) {
      updateTradingPeriod(mkt.trading_day());
    }

    std::string tsStr = mkt.trading_day() + mkt.exchange_timestamp();
    pt::ptime ts = CedarTimeHelper::strToPTime("%Y%m%d%H%M%S%F", tsStr);

    return onTickUpdate(mkt, ts, res);
  }

  //this is only used for real time tick update !!!
  bool onRealTimeDataTickUpdate(MarketUpdate &mkt, RangeStatResult &res) {
    std::string tsStr = CedarTimeHelper::getCurTimeStamp();
    pt::ptime ts = CedarTimeHelper::strToPTime("%H%M%S%F", tsStr);

    return onTickUpdate(mkt, ts, res);
  }

private:

  bool updateTradingPeriod(std::string tradingDay) {
    //YYYYMMDD
    int year = atoi(tradingDay.substr(0, 4).c_str());
    int month = atoi(tradingDay.substr(4, 2).c_str());
    int day = atoi(tradingDay.substr(6, 2).c_str());
    gg::date newDate(year, month, day);
    for (unsigned i = 0; i < periods.size(); i++) {
      periods[periodIdx].first =
        pt::ptime(newDate, periods[periodIdx].first.time_of_day());
      periods[periodIdx].second =
        pt::ptime(newDate, periods[periodIdx].second.time_of_day());
    }

    updateTradingDay = tradingDay;

    return true;
  }

  bool onTickUpdate(MarketUpdate &mkt, pt::ptime ts, RangeStatResult &res) {
    if (ts < periods[periodIdx].first) {
      return false;
    }

    while (ts >= periods[periodIdx].second) {
      periodIdx++;

      if (periodIdx >= periods.size()) {
        return false;
      } else if (ts > periods[periodIdx].first) {
        res = rangeStat;
        rangeStat.init(mkt);

        return true;
      }
    }

    if (periods[periodIdx].first <= ts && ts < periods[periodIdx].second) {
      rangeStat.update(mkt);
      rangeStat.start = periods[periodIdx].first;
      rangeStat.end = periods[periodIdx].second;
    }

    return false;
  }

  const static int offset = 20;

  int frequency;
  pt::time_duration rangePeriod, endOffset;
  ExchangeType exchange;

  unsigned periodIdx;
  std::vector<std::pair<pt::ptime, pt::ptime>> periods;

  RangeStatResult rangeStat;
  std::string updateTradingDay;
};

//class RangeStatCollector {
//public:
//  RangeStatCollector(int rangeSeconds, std::string mss, std::string mse,
//    std::string afterStart, std::string afterEnd) :
//    morningSessionStart(mss), morningSessionEnd(mse),
//    afternoonSessionStart(afterStart), afternoonSessionEnd(afterEnd),
//    rangePeriod(pt::seconds(rangeSeconds)) {
//  }
//
//  bool onTickUpdate(MarketUpdate &mkt, RangeStatResult &res) {
//    std::string tmp;
//    std::string tsStr = mkt.trading_day() + mkt.exchange_timestamp();
//    pt::ptime ts = CedarTimeHelper::strToPTime("%Y%m%d%H%M%S%F", tsStr);
//
//    tmp = mkt.trading_day() + afternoonSessionStart;
//    afternoonStart = CedarTimeHelper::strToPTime("%Y%m%d%H:%M:%S", tmp);
//    tmp = mkt.trading_day() + afternoonSessionEnd;
//    afternoonEnd = CedarTimeHelper::strToPTime("%Y%m%d%H:%M:%S", tmp);
//    tmp = mkt.trading_day() + morningSessionEnd;
//    morningEnd = CedarTimeHelper::strToPTime("%Y%m%d%H:%M:%S", tmp);
//
//    //LOG(INFO) << "curTimePeriod start " << curTimePeriod.start;
//    //LOG(INFO) << "curTimePeriod end " << curTimePeriod.end;
//    //LOG(INFO) << "ts " << ts;
//
//    if (rangeStat.state == RangeStatResult::Uninitialize) {
//      rangeStat.init(mkt);
//      setNewMarketDay(mkt.trading_day());
//    }
//
//    while (ts > curTimePeriod.end) {
//      if (tradingDay != mkt.trading_day()) {
//        setNewMarketDay(mkt.trading_day());
//      } else {
//        curTimePeriod.start += rangePeriod;
//        curTimePeriod.end += rangePeriod;
//
//        if (curTimePeriod.start == morningEnd) {
//          //setToAfternoonSession();
//          curTimePeriod.start = afternoonStart;
//          curTimePeriod.end = curTimePeriod.start + rangePeriod;
//        } else if (curTimePeriod.start == afternoonEnd) {
//          //skip to next day
//          setNewMarketDay(addOneDay(mkt.trading_day()));
//        }
//      }
//
//      if (ts >= curTimePeriod.start && ts <= curTimePeriod.end) {
//        res = rangeStat;
//        rangeStat.init(mkt);
//        LOG(INFO) << "inRange";
//        return true;
//      }
//    }
//
//    if (ts < curTimePeriod.start)
//      return false;
//
//    if (curTimePeriod.start <= ts && ts <=  curTimePeriod.end) {
//      rangeStat.start = curTimePeriod.start;
//      rangeStat.end = curTimePeriod.end;
//      rangeStat.update(mkt);
//    }
//
//    return false;
//  }
//
//private:
//  bool setNewMarketDay(std::string dayStr) {
//    std::string tsStr = dayStr + morningSessionStart;
//    curTimePeriod.start = CedarTimeHelper::strToPTime(
//        "%Y%m%d%H:%M:%S", tsStr);
//    curTimePeriod.end = curTimePeriod.start + rangePeriod;
//    tradingDay = dayStr;
//
//    return true;
//  }
//
//  std::string addOneDay(std::string dayStr) {
//    pt::ptime ptm = CedarTimeHelper::strToPTime("%Y%m%d", dayStr);
//    ptm += pt::hours(24);
//    return CedarTimeHelper::ptimeToStr("%Y%m%d", ptm);
//  }
//
//  RangeStatResult rangeStat;
//  TimeInterval curTimePeriod;
//
//  std::string tradingDay;
//  std::string morningSessionStart, morningSessionEnd;
//  std::string afternoonSessionStart, afternoonSessionEnd;
//
//  pt::ptime morningEnd, afternoonStart, afternoonEnd;
//  pt::time_duration rangePeriod;
//};

#endif
