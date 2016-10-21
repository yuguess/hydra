#ifndef TRANSAC_LOGGER
#define TRANSAC_LOGGER

#include <fstream>
#include <iostream>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "json/json.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "CedarTimeHelper.h"
#include "CedarJsonConfig.h"

namespace pt = boost::posix_time;

class TransacLogger {
public:
  TransacLogger() : firstFlag(true),
    minStartTime(boost::gregorian::date(2099, 1, 1)),
    maxEndTime(boost::gregorian::date(1980, 1, 1)) {

    CedarJsonConfig::getInstance().getStringByPath("CedarLogger.AppName", app);
    if (CedarJsonConfig::getInstance().hasMember("Backtest")) {
      getBacktestRange();
    }
  }

  int enter(TradeDirection buySell, std::string code, int qty, double refPrice,
      std::string ts) {

    trans["Qty"] = qty;
    trans["Direction"] = EnumToString::toString(buySell);
    trans["Timestamp"] = ts;
    trans["Price"] = refPrice;
    trans["Code"] = code;

    LOG(INFO) << trans;

    root["Transactions"].append(trans);

    return 0;
  }

  bool saveToFile() {
    std::string fileName = app + "_" +
      CedarTimeHelper::timestampFormatString("%Y%m%d%H%M%S");
    std::ofstream out;
    out.open(fileName);

    Json::StyledWriter sytledWriter;
    out << sytledWriter.write(root);
    out.close();

    return true;
  }

private:
  bool getBacktestRange() {
    std::vector<std::string> streamNames;
    CedarJsonConfig::getInstance().getStringArrayWithTag(streamNames,
        "Backtest.Streams");

    for (unsigned i = 0; i < streamNames.size(); i++) {
      minStartTime =
        std::min(minStartTime, getRangePTime(streamNames[i], "Start"));
      maxEndTime =
        std::max(maxEndTime, getRangePTime(streamNames[i], "End"));
    }

    root["Range"]["Start"] =
      CedarTimeHelper::ptimeToStr("%Y%m%d", minStartTime);
    root["Range"]["End"] =
      CedarTimeHelper::ptimeToStr("%Y%m%d", maxEndTime);

    return true;
  }

  pt::ptime getRangePTime(std::string streamName, std::string tag) {
    std::string path = "Backtest." + streamName + ".Range." + tag;
    std::string dateStr;

    if (!CedarJsonConfig::getInstance().hasMember(path)) {
      LOG(FATAL) << "TransacLogger need to access path " << path
        << " check your backtest section in config file, it "
        << " need to have Range{Start:YYYYMMDD, End:YYYYMMDD";
    }

    CedarJsonConfig::getInstance().getStringByPath(path, dateStr);
    return CedarTimeHelper::strToPTime("%Y%m%d", dateStr);
  }

  std::string app;
  bool firstFlag;
  TradeDirection preBuySell;
  double preRefPrice, preTs;
  Json::Value trans, root;
  pt::ptime minStartTime, maxEndTime;
};

#endif

