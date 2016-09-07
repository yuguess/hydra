#ifndef DATA_ADAPTER_H
#define DATA_ADAPTER_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "json/json.h"

struct TimeSeriesData {
  MessageBase msg;
  boost::posix_time::ptime ts;
  std::string streamName;
};

class DataAdapter {
public:
  virtual int init(std::string &, Json::Value&) = 0;
  virtual bool getNextData(TimeSeriesData&) = 0;
  virtual int close() = 0;
};

#endif
