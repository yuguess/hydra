#ifndef CEDAR_TIME_HELPER_H
#define CEDAR_TIME_HELPER_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <boost/date_time/posix_time/posix_time.hpp>

class CedarTimeHelper {
public:

  //seconds since epoch
  static std::string timestampString() {
    time_t sec;
    time(&sec);
    std::stringstream ss;
    ss << sec;
    return ss.str();
  }

  static std::string timestampFormatString(std::string fmt) {
    char buf[128];
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    strftime(buf, 128, fmt.c_str(), &tm);
    return std::string(buf);
  }

  //return current local time HHMMSSmmm
  static std::string getCurTimeStamp() {
    char currentTime[10];
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    int milli = curTime.tv_usec / 1000;
    char buffer [10];
    strftime(buffer, sizeof(buffer), "%H%M%S", localtime(&curTime.tv_sec));
    sprintf(currentTime, "%s%d", buffer, milli);

    return currentTime;
  }

  static boost::posix_time::ptime getCurPTime() {
    return boost::posix_time::second_clock::local_time();
  }

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
