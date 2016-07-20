#ifndef BASIC_FUTURES_DATA_ADAPTER_H
#define BASIC_FUTURES_DATA_ADAPTER_H

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "DataAdapterHelper.h"

namespace fs = boost::filesystem;
namespace pt = boost::posix_time;

class BasicFuturesDataAdapter : public DataAdapter {

public:
  int init(std::string &stream, Json::Value &jsonConfigObj) {
    std::string startStr = jsonConfigObj["Range"]["Start"].asString();
    std::string endStr = jsonConfigObj["Range"]["End"].asString();

    startDate = DataAdapterHelper::strToPTime("%Y%m%d", startStr);
    endDate = DataAdapterHelper::strToPTime("%Y%m%d", endStr);

    std::string code = jsonConfigObj["Code"].asString();
    std::string homeDir = jsonConfigObj["FileAddress"].asString();

    streamName = stream;

    setupDateToFileMap(homeDir + "/" + code);

    return 0;
  }

  bool getNextData(TimeSeriesData &tsData) {
    static boost::posix_time::ptime curDate = startDate;
    static boost::gregorian::days oneDay(1);
    static std::ifstream ifs(getDataFileStr(curDate), std::ifstream::in);
    std::string line;

    LOG(INFO) << "getNext for BasicFutuersData";
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

      ifs.open(getDataFileStr(startDate), std::ifstream::in);
    }
    return false;
  }

  int close() {
    return 0;
  }

private:
  std::string getDataFileStr(boost::posix_time::ptime date) {
    std::string dateStr = DataAdapterHelper::ptimeToStr("%Y%m%d", date);
    //std::string fileStr = homeDir + "/" + exchange + code + "/" + dateStr +
    //  "_" + exchange + code + "_" + exchangeAbbr + "_L1.txt";
    //LOG(INFO) << fileStr;
    //getchar();
    return dateStr;
  }

  int setupDateToFileMap(std::string dirPath) {
    if (!fs::exists(dirPath)) {
      LOG(FATAL) << "can't find path " << dirPath;
    }

    fs::directory_iterator end;
    for (fs::directory_iterator it(dirPath); it != end; it++) {
      LOG(INFO) << it->path().filename();
    }

    getchar();
    return 0;
  }

  int lineToMarketUpdate(std::string &line, TimeSeriesData &tsData) {
    LOG(INFO) << line;
    getchar();
    //MarketUpdate mkt;
    //std::vector<std::string> args;
    //boost::split(args, line, boost::is_any_of(","));
    ////mkt.set_exchange(args[0]);
    //mkt.set_code(args[1]);
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
  }


  pt::ptime startDate;
  pt::ptime endDate;
  std::string streamName;
};

#endif
