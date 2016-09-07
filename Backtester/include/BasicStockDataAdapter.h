#ifndef BASIC_STOCK_DATA_ADAPTER_H
#define BASIC_STOCK_DATA_ADAPTER_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "CedarTimeHelper.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>

class BasicStockDataAdapter : public DataAdapter {
public:
  BasicStockDataAdapter() : oneDay(1) {}

  int init(std::string &stream, Json::Value &jsonConfigObj) {
    std::string startStr = jsonConfigObj["Range"]["Start"].asString();
    std::string endStr = jsonConfigObj["Range"]["End"].asString();

    startDate = CedarTimeHelper::strToPTime("%Y%m%d", startStr);
    endDate = CedarTimeHelper::strToPTime("%Y%m%d", endStr);

    //code is like 000001.SZ format
    code = jsonConfigObj["Code"].asString().substr(0, 6);
    exchange = jsonConfigObj["Code"].asString().substr(7, 2);
    freq = jsonConfigObj["Frequency"].asString();
    homeDir = jsonConfigObj["FileAddress"].asString();
    streamName = stream;

    curDate = startDate;
    ifs.open(getDataFileStr(startDate), std::ifstream::in);

    return 0;
  }

  bool getNextData(TimeSeriesData &tsData) {

    std::string line;

    while (true) {
      if (ifs.good()) {
        getline(ifs, line);
        lineToMarketUpdate(line, tsData);
        return true;
      }

      if (ifs.is_open()) {
        ifs.close();
      }

      curDate += oneDay;

      if (curDate > endDate)
        break;

      ifs.open(getDataFileStr(curDate), std::ifstream::in);
    }

    return false;
  }

  int close() {
    return 0;
  }

private:
  int lineToMarketUpdate(std::string &line, TimeSeriesData &tsData) {
    MarketUpdate mkt;
    std::vector<std::string> args;
    boost::split(args, line, boost::is_any_of(","));
    //mkt.set_exchange(args[0]);
    //mkt.set_code();
    ////args[2] format like YYYY-mm-dd HH:MM:SS.mmm
    //mkt.set_trading_day(args[2].substr(0, 10));
    //mkt.set_exchange_timestamp(args[2].substr(11, 12));

    //mkt.set_last_price(std::stoi(args[3]));
    //mkt.set_num_trades(std::stoi(args[4]));
    //mkt.set_turnover(std::stof(args[5]));
    //mkt.set_volume(std::stoi(args[6]));

    //for (unsigned int i = 0; i < LEVEL; i++)
    //  mkt.add_bid_price(std::stof(args[8 + i]));

    //for (unsigned int i = 0; i < LEVEL; i++)
    //  mkt.add_ask_price(std::stof(args[13 + i]));

    //for (unsigned int i = 0; i < LEVEL; i++)
    //  mkt.add_bid_volume(std::stoi(args[18 + i]));

    //for (unsigned int i = 0; i < LEVEL; i++)
    //  mkt.add_ask_volume(std::stoi(args[23 + i]));

    //tsData.msg =
    //  ProtoBufHelper::toMessageBase<MarketUpdate>(TYPE_MARKETUPDATE, mkt);
    //tsData.streamName = streamName;
    //tsData.ts = boost::posix_time::time_from_string(args[2]);

    return 0;
  }

  std::string getDataFileStr(boost::posix_time::ptime date) {
    std::string dateStr = CedarTimeHelper::ptimeToStr("%Y%m%d", date);
    LOG(INFO) << dateStr;
    std::string fileStr =
      homeDir + "/" + freq + "/" + dateStr + "/" + code + "." + exchange;
    LOG(INFO) << fileStr;
    return fileStr;
  }

  const static int LEVEL = 5;

  boost::posix_time::ptime startDate;
  boost::posix_time::ptime endDate;
  std::string homeDir, exchange, code, streamName, freq;
  int backtestDays;

  boost::posix_time::ptime curDate;
  boost::gregorian::days oneDay;
  std::ifstream ifs;
};

#endif
