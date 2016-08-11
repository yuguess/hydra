#ifndef BASIC_FUTURES_DATA_ADAPTER_H
#define BASIC_FUTURES_DATA_ADAPTER_H

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "CedarTimeHelper.h"
#include "CedarHelper.h"
#include "EnumStringMap.h"

namespace fs = boost::filesystem;
namespace pt = boost::posix_time;

class BasicFuturesDataAdapter : public DataAdapter {

public:
  int init(std::string &stream, Json::Value &jsonConfigObj) {
    std::string startStr = jsonConfigObj["Range"]["Start"].asString();
    std::string endStr = jsonConfigObj["Range"]["End"].asString();

    startDate = CedarTimeHelper::strToPTime("%Y%m%d", startStr);
    endDate = CedarTimeHelper::strToPTime("%Y%m%d", endStr);
    code = jsonConfigObj["Code"].asString();

    std::string homeDir = jsonConfigObj["FileAddress"].asString();
    std::string exchangeStr = jsonConfigObj["Exchange"].asString();
    exchange = StringToEnum::toExchangeType(exchangeStr);

    streamName = stream;

    setupDateToFileMap(homeDir + "/" + code);

    return 0;
  }

  bool getNextData(TimeSeriesData &tsData) {
    static pt::ptime curDate = startDate;
    static std::string curDateStr =
      CedarTimeHelper::ptimeToStr("%Y%m%d", curDate);
    static boost::gregorian::days oneDay(1);
    static std::ifstream ifs(getDataFileStr(startDate), std::ifstream::in);
    std::string line;

    while (true) {
      if (ifs.good()) {
        getline(ifs, line);
        if (lineToMarketUpdate(line, tsData, curDateStr))
          return true;
        else
          continue;
      }

      if (ifs.is_open()) {
        ifs.close();
      }

      curDate += oneDay;
      curDateStr = CedarTimeHelper::ptimeToStr("%Y%m%d", curDate);

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
  std::string getDataFileStr(boost::posix_time::ptime date) {
    std::string dateStr = CedarTimeHelper::ptimeToStr("%Y%m%d", date);
    if (dateToFile.find(dateStr) == dateToFile.end())
      return "";
    return dateToFile[dateStr];
  }

  int setupDateToFileMap(std::string dirPath) {
    if (!fs::exists(dirPath)) {
      LOG(FATAL) << "can't find path " << dirPath;
    }

    fs::directory_iterator end;
    for (fs::directory_iterator it(dirPath); it != end; it++) {
      //name format is like 20110101_IF1503_CFFEX_L1.txt
      std::string dateStr = it->path().filename().string();
      dateToFile[dateStr.substr(0, 8)] =  dirPath + "/" + dateStr;
    }

    return 0;
  }

  bool lineToMarketUpdate(std::string &line,
      TimeSeriesData &tsData, std::string &dateStr) {
    MarketUpdate mkt;
    std::vector<std::string> args;
    boost::split(args, line, boost::is_any_of(" "), boost::token_compress_on);

    if (args.size() == 1)
      return false;

    mkt.set_code(code);
    mkt.set_exchange(exchange);
    mkt.set_last_price(std::stof(args[0]));
    mkt.set_highest_price(std::stof(args[1]));
    mkt.set_lowest_price(std::stof(args[2]));
    mkt.set_volume(std::stol(args[3]));
    mkt.set_turnover(std::stol(args[4]));
    mkt.set_trading_day(dateStr);

    mkt.add_bid_price(std::stof(args[9]));
    mkt.add_bid_volume(std::stoi(args[10]));
    mkt.add_ask_price(std::stof(args[11]));
    mkt.add_ask_volume(std::stoi(args[12]));

    std::string tsStr = dateStr + " " + args[8];
    tsData.ts = CedarTimeHelper::strToPTime("%Y%m%d %H:%M:%S", tsStr) +
      pt::milliseconds(std::stoi(args[9]));

    tsData.msg =
      ProtoBufHelper::toMessageBase<MarketUpdate>(TYPE_MARKETUPDATE, mkt);
    tsData.streamName = streamName;

    return true;
  }

  std::map<std::string, std::string> dateToFile;
  pt::ptime startDate;
  pt::ptime endDate;
  std::string streamName;
  std::string code;
  ExchangeType exchange;
};

#endif
