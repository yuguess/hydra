#ifndef DATA_ADAPTER_HELPER_H
#define DATA_ADAPTER_HELPER_H

#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>

class DataAdapterHelper {

public:

  static boost::posix_time::ptime strToPTime(std::string fmt,
      std::string &timeStr) {
    static std::stringstream ss;
    static boost::posix_time::time_input_facet *facet = NULL;
    if (!facet) {
      facet = new boost::posix_time::time_input_facet(1);
      ss.imbue(std::locale(std::locale(), facet));
    }

    facet->format(fmt.c_str());
    ss.str(timeStr);
    boost::posix_time::ptime ts;

    ss >> ts;
    ss.clear();
    return ts;
  }

  static std::string ptimeToStr(std::string fmt, boost::posix_time::ptime &pt) {
    static std::stringstream ss;
    static boost::posix_time::time_facet *facet = NULL;
    if (!facet) {
      facet = new boost::posix_time::time_facet();
      ss.imbue(std::locale(std::locale::classic(), facet));
    }

    facet->format(fmt.c_str());
    ss << pt;
    std::string tsStr = ss.str();

    ss.str("");
    return tsStr;
  }
};

#endif
