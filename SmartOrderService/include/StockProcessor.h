#ifndef STOCK_PROCESSOR_H
#define STOCK_PROCESSOR_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include <chrono>
#include <boost/circular_buffer.hpp>
#include "easylogging++.h"

typedef std::chrono::high_resolution_clock::time_point TimeStamp;

struct DataBundle {
  MarketUpdate mkdt;
  TimeStamp ts;
};

class StockProcessor {
public:
  StockProcessor() : mktUpdtCnt(1), dataBuf(dataBufSize) {};
  int onMarketUpdate(MarketUpdate);
  int init(std::string argCode);

private:
  const static int  initBufSize = 2;
  const static int dataBufSize = 100;
  const static double priceAnomalyThreshold;
  
  std::string code;
  int mktUpdtCnt;

  int priceAnomaly(MarketUpdate &);
  boost::circular_buffer<DataBundle> dataBuf;
};

#endif
