#ifndef PERSISTENT_STATE_H
#define PERSISTENT_STATE_H

#include "json/reader.h"
#include "json/writer.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <fstream>
#include <algorithm>
#include "CedarTimeHelper.h"
#include "CedarJsonConfig.h"
#include "CedarHelper.h"

namespace fs = boost::filesystem;

class PersistentState {
public:
  static bool load(Json::Value &val) {
    std::string dir, app;
    CedarJsonConfig::getInstance().getStringByPath("PersistentState.Dir", dir);
    CedarJsonConfig::getInstance().getStringByPath("CedarLogger.AppName", app);

    boost::posix_time::ptime latestTS(boost::gregorian::date(1980, 1, 1));
    fs::directory_iterator end;
    for (fs::directory_iterator it(dir); it != end; it++) {
      std::string stateStr = it->path().filename().string();

      std::vector<std::string> res;
      CedarHelper::stringSplit(stateStr, '_', res);
      //fmt should be like XXX_TIMESTAMP
      if (res.size() != 2) {
        LOG(FATAL) << "try to recover state from invalid json file"<< stateStr;
      }

      std::string ts = res[1];
      latestTS = std::max(latestTS,
          CedarTimeHelper::strToPTime("%Y%m%d%H%M%S", ts));
    }

    std::string latestJson = dir + app + "_" +
      CedarTimeHelper::ptimeToStr("%Y%m%d%H%M%S", latestTS);

    std::ifstream fileStream(latestJson, std::ifstream::binary);
    Json::Reader reader;
    if (!reader.parse(fileStream, jsonResult)) {
      LOG(FATAL) << "All state file must have app_YYYYmmddHHMMSS format"
        << "Please check your path " << dir << "Load config file "
        << latestJson << " error";
    }

    return true;
  }

  static bool save(Json::Value &val) {
    std::string app, statePath;
    CedarJsonConfig::getInstance().getStringByPath("CedarLogger.AppName", app);
    CedarJsonConfig::getInstance().getStringByPath("PersistentState.Dir",
      statePath);

    std::string stateFileStr = statePath + app + "_" +
      CedarTimeHelper::timestampFormatString("%Y%m%d%H%M%S");

    std::ofstream out;
    out.open(stateFileStr);

    Json::StyledWriter sytledWriter;
    out << sytledWriter.write(jsonState);
    out.close();
    return true;
  }
};

#endif
