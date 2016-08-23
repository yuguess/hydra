#ifndef RANGE_DATA_ADAPTER
#define RANGE_DATA_ADAPTER

#include <boost/algorithm/string.hpp>
#include "DataAdapter.h"
#include "CedarTimeHelper.h"
#include "CedarHelper.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"

namespace pt = boost::posix_time;

class RangeDataAdapter : public DataAdapter {
  int init(std::string &stream, Json::Value &config) {
    std::string startStr = config["Range"]["Start"].asString();
    std::string endStr = config["Range"]["End"].asString();

    startDate = CedarTimeHelper::strToPTime("%Y%m%d", startStr);
    endDate = CedarTimeHelper::strToPTime("%Y%m%d", endStr);

    code = config["Code"].asString();
    homeDir = config["FileAddress"].asString();
    freq = config["Frequency"].asString();
    streamName = stream;
    //std::string exchangeStr = jsonConfigObj["Exchange"].asString();
    //exchange = StringToEnum::toExchangeType(exchangeStr);

    //setupDateToFileMap(homeDir + "/" + code);

    return 0;
  }

  bool getNextData(TimeSeriesData &tsData) {
    static pt::ptime curDate = startDate;
    static std::string curDateStr =
      CedarTimeHelper::ptimeToStr("%Y%m%d", curDate);
    static std::ifstream ifs(getDataFileStr(startDate), std::ifstream::in);
    static boost::gregorian::days oneDay(1);
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
    std::string fileStr = homeDir + "/" + freq + "/" + dateStr + "/" + code;
    return fileStr;
  }

  bool lineToMarketUpdate(std::string &line,
    TimeSeriesData &tsData, std::string &dateStr) {
    RangeStat rangeStat;

    std::vector<std::string> args;
    boost::split(args, line, boost::is_any_of(","), boost::token_compress_on);

    if (args.size() == 1)
      return false;

    rangeStat.set_code(code);
    rangeStat.set_open(std::stod(args[3]));
    rangeStat.set_high(std::stod(args[4]));
    rangeStat.set_low(std::stod(args[5]));
    rangeStat.set_close(std::stod(args[6]));
    rangeStat.set_stream(streamName);

    std::string tsStr = args[0] + args[2];
    tsData.ts = CedarTimeHelper::strToPTime("%Y%m%d%H:%M:%S", tsStr);
    tsData.msg = ProtoBufHelper::toMessageBase<RangeStat>(
        TYPE_RANGE_STAT, rangeStat);
    tsData.streamName = streamName;

    return true;
  }

  std::string code, homeDir, freq, streamName;
  pt::ptime startDate, endDate;
};

#endif

