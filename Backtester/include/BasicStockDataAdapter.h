#ifndef BASIC_STOCK_DATA_ADAPTER_H
#define BASIC_STOCK_DATA_ADAPTER_H

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "DataAdapter.h"

class BasicStockDataAdapter : public DataAdapter {
public:
  int initWithArgList(std::string code, std::string &argList) {
    std::string backtestDaysStr;

    CedarJsonConfig::getInstance().getStringByPath("BasicStock.DataHomeDir",
      homeDir);
    CedarJsonConfig::getInstance().getStringByPath("BasicStock.StartDate",
      startDate);
    CedarJsonConfig::getInstance().getStringByPath("BasicStock.Length",
      backtestDaysStr);

    backtestDays = std::stoi(backtestDaysStr);
    std::vector<std::string> args;
    boost::split(args, argList, boost::is_any_of(";"));
    exchange = args[0];
    dataType = args[1];

    return 0;
  }

  int getNextData(MarketUpdate &mktUpdt) {

    std::string nextDataFile;
    getNextDataFilePath(nextDataFile);
    static std::ifstream ifs;
    ifs.open(nextDataFile, std::ifstream::in);

    static int processedFile = 0;
    static bool processFirstLineFlag = false;
    std::string line;

    while (true) {
      if (ifs.good()) {
        getline(ifs, line);
        if (!processFirstLineFlag)
          processFirstLineFlag = true;
          continue;

        return lineToMarketUpdate(line, mktUpdt);
      }

      ifs.close();
      processFirstLineFlag = false;
      processedFile++;

      if (processedFile >= backtestDays)
        break;

      getNextDataFilePath(nextDataFile);
      ifs.open(nextDataFile) ;
    }

    return 0;
  }

  int close() {
    //close fd
    return 0;
  }

  int lineToMarketUpdate(std::string &line, MarketUpdate &mkt) {
    std::vector<std::string> args;
    boost::split(args, line, boost::is_any_of(","));
    mkt.set_exchange(args[0]);
    mkt.set_code(args[1]);
    //args[2] format like YYYY-mm-dd HH:MM:SS.mmm
    mkt.set_trading_day(args[2].substr(0, 10));
    mkt.set_exchange_timestamp(args[2].substr(11, 12));

    mkt.set_last_price(std::stoi(args[3]));
    mkt.set_num_trades(std::stoi(args[4]));
    mkt.set_turnover(std::stof(args[5]));
    mkt.set_volume(std::stoi(args[6]));

    for (unsigned int i = 0; i < LEVEL; i++)
      mkt.add_bid_price(std::stof(args[8 + i]));

    for (unsigned int i = 0; i < LEVEL; i++)
      mkt.add_ask_price(std::stof(args[13 + i]));

    for (unsigned int i = 0; i < LEVEL; i++)
      mkt.add_bid_volume(std::stoi(args[18 + i]));

    for (unsigned int i = 0; i < LEVEL; i++)
      mkt.add_ask_volume(std::stoi(args[23 + i]));
  }

private:
  boost::posix_time::ptime startDateToPTime() {
    return boost::posix_time::time_from_string(startDate + " 00:00:00.000");
  }

  boost::posix_time::ptime todayPTime() {
    boost::posix_time::ptime today(boost::gregorian::day_clock::local_day());
    return today;
  }

  int getNextDataFilePath(std::string &filePath) {
    static boost::posix_time::ptime currentPTime = startDateToPTime();
    static boost::posix_time::ptime today = todayPTime();
    static boost::gregorian::days oneDay(1);

    while (currentPTime < today) {
      genFilePath(currentPTime, filePath);
      if (access(filePath.c_str(), F_OK) != -1)
        return 0;
      else {
        currentPTime = currentPTime + oneDay;
      }
    }

    LOG(FATAL) << "check your backtest data file start date and length";
    return -1;
  }

  int genFilePath(boost::posix_time::ptime &ptime, std::string &filePath) {
    filePath = homeDir + "/" + "Stk_Tick_";
  }

  const static int LEVEL = 5;

  std::string homeDir;
  std::string startDate;
  std::string dataType;
  std::string exchange;
  int backtestDays;
};

#endif
