#ifndef WIND_DATA_COLLECTOR_H
#define WIND_DATA_COLLECTOR_H

#include "TDFAPI.h"
#include <sys/stat.h>
#include <sys/stat.h>
#include "unordered_set"

class WindDataCollector {

public:
  WindDataCollector();
  ~WindDataCollector();
  int start();
  int close();
  static void RecvData(THANDLE hTdf, TDF_MSG* pMsgHead);
  static void RecvSys(THANDLE hTdf, TDF_MSG* pSysMsg);
  static void DumpMarketToLog(TDF_MARKET_DATA*, int);
  static void DumpIndexToLog(TDF_INDEX_DATA*, int);
  static void DumpTransToLog(TDF_TRANSACTION*, int);

private:
  TDF_OPEN_SETTING settings;
  TDF_ERR nErr;
  THANDLE nTDF;
  bool closeFlag;
  static std::string targetDir;
  static std::set<std::string> CodeRecorder;

  const static int priceLevel = 10;
};

#endif
