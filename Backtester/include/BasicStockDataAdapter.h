#ifndef BASIC_STOCK_DATA_ADAPTER_H
#define BASIC_STOCK_DATA_ADAPTER_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "DataAdapter.h"

class BasicStockDataAdapter : public DataAdapter {
public:
  int initWithArgList(std::string code, std::string &argList) {
    CedarJsonConfig::getInstance().getStringByPath("BasicStock.DataHomeDir",
      homeDir);
    CedarJsonConfig::getInstance().getStringByPath("BasicStock.StartDate",
      startDate);

    std::vector<std::string> args;
    boost::split(args, argList, ';');
    exchange = args[0];
    dataType = args[1];
     
    return 0;
  }

  int getNextData(MarketUpdate &mktUpdt) {

    getNextDataFilePath();
    static fstream = ;
    static processedFile = 0;

    while ( < length) {
      if (goodstate()) {
        getline(fstream, line);
        if (!processFirstLineFlag) 
          processFirstLineFlag = true;
          continue;
       
        return lineToMarketUpdate();
      }
      
      close();
      getNextDataFilePath();
      fstream = ;
      processed++;
    }
    
    std::string year, month, day;

    while (processeDay < length) {
      processDate += 1
      if (processDate > today()) {
        LOG(FATAL) <<  
      }
      
    }
    
    //get home dir
    //build string path

    std::string fileName 
    
    return 0;
  }

  int close() {
    //close fd
    return 0;
  }

private:
  boost::posix_time::ptime startDateToPTime() {
    std::locale loc = std::locale(std::locale::classic(), 
        new boost::posix_time::time_input_facet("%Y%m%d"));
    std::isstringstream is(startDate);
    is.imbue(loc);

    return boost::posix_time::time_from_string(is);
  }

  int getNextDataFilePath(std::string &filePath) {
    
    static boost::posix_time::ptime currentPTime = startDateToPTime();
    
    while (currentPTime < today()) {
      genFilePath(currentDay, filePath);
      if check filePath exist 
        return 0;
      else
        currentPTime = curentPTime + 1;
    }

    LOG(FATAL) << 
    return -1;
  }

  int genFilePath(std::string &dateString, std::string &filePath) {
    filePath = homeDir + "/" + "Stk_Tick_" + ; 
  }

  std::string homeDir;
  std::string startDate;
  std::string dataType;
  std::string exchange;
};

#endif
