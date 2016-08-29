#ifndef DAY_DATA_ADAPTER
#define DAY_DATA_ADAPTER

#include "CedarTimeHelper.h"
#include "CedarHelper.h"
#include "DataAdapter.h"

namespace pt = boost::posix_time;

class DayDataAdapter : public DataAdapter {
  int init(std::string &stream, Json::Value &config) {
    std::string startStr = config["Range"]["Start"].asString();
    std::string endStr = config["Range"]["End"].asString();

    startDate = CedarTimeHelper::strToPTime("%Y%m%d", startStr);
    endDate = CedarTimeHelper::strToPTime("%Y%m%d", endStr);

    code = config["Code"].asString();
    homeDir = config["FileAddress"].asString();
    freq = config["Frequency"].asString();
    streamName = stream;

    return 0;
  }

  bool getNextData(TimeSeriesData &tsData) {
    static pt::ptime curDate;
    static std::string curDateStr =
      CedarTimeHelper::ptimeToStr("%Y%m%d", curDate);
    static std::ifstream ifs(getDataFileStr(), std::ifstream::in);
    std::string line;
    static bool firstReadFlag = true;

    while (true) {
      pt::ptime date;

      if (ifs.good()) {
        if (firstReadFlag) {
          firstReadFlag = false;
          do {
            getline(ifs, line);
            if (!ifs.good())
              return false;
            lineToMarketUpdate(line, tsData, curDate);
          } while(curDate < startDate);
          return true;
        }

        getline(ifs, line);
        if (!ifs.good())
          return false;

        if (lineToMarketUpdate(line, tsData, curDate) && curDate <= endDate) {
          return true;
        }
      }

      if (ifs.is_open()) {
        ifs.close();
      }

      return false;
    }

    return false;
  }

  int close() {
    return 0;
  }

private:
  std::string getDataFileStr() {
    std::string fileStr = homeDir + "/" + freq + "/" + code;
    return fileStr;
  }

  bool lineToMarketUpdate(std::string &line,
      TimeSeriesData &tsData, pt::ptime &curDate) {

    RangeStat rangeStat;
    std::vector<std::string> args;
    boost::split(args, line, boost::is_any_of(","), boost::token_compress_on);

    if (args.size() == 1) {
      LOG(INFO) << " there is error data " << line;
      return false;
    }

    std::string tsStr = args[0] + " 15:30:00";

    rangeStat.set_code(code);
    rangeStat.set_open(std::stod(args[1]));
    rangeStat.set_high(std::stod(args[2]));
    rangeStat.set_low(std::stod(args[3]));
    rangeStat.set_close(std::stod(args[4]));
    rangeStat.set_volume(std::stoul(args[5]));
    rangeStat.set_stream(streamName);
    rangeStat.set_timestamp(tsStr);

    curDate = CedarTimeHelper::strToPTime("%Y-%m-%d", args[0]);

    //for day data, we append market close time as its ts
    tsData.ts = CedarTimeHelper::strToPTime("%Y-%m-%d %H:%M:%S", tsStr);
    tsData.msg =
      ProtoBufHelper::toMessageBase<RangeStat>(TYPE_RANGE_STAT, rangeStat);
    tsData.streamName = streamName;

    return true;
  }

  std::string code, homeDir, freq, streamName;
  pt::ptime startDate, endDate;
};

#endif
