#include "Backtester.h"
#include "CedarJsonConfig.h"
#include "DataAdapter.h"
#include "DataAdapterFactory.h"

#include <iostream>
#include <vector>

int Backtester::run() {

  std::vector<std::string> codes;
  std::vector<std::string> adapterTypes;
  CedarJsonConfig::getInstance().getStringArrayWithTag(codes,
    "Backtester.Ticker", "code");
  CedarJsonConfig::getInstance().getStringArrayWithTag(adapterTypes,
    "Backtester.Ticker", "adapter");

  std::vector<std::shared_ptr<DataAdapter>> adapters;
  for (unsigned int i = 0; i < codes.size(); i++) {
    adapters.push_back(AdapterFactory::createAdapter(adapterTypes[i]));
  }

  for (unsigned int i = 0; i < adapters.size(); i++ ) {
    MarketUpdate mktUpdt;
    if (adapters[i]->getNextData(mktUpdt) != 0) {
      LOG(ERROR) << "there is a emtpy data soruce";
    } else {
      pq.push(mktUpdt);
    }
  }

  MarketUpdate cur = pq.top();

  do {
    //while (pri,top() <= curtime) {
    //  updateMatchingEngine();
    //  callregister function;
    //}

    curtime += 0.5
  } while (true);

  //for (int i
  //
  //DataAdapter();
  //init
  //call get next data for every security
  //push into pq
  //set pq top as the starting time
  //Stock market close

  while (true) {
    //getNextData();
    //caculate ts put into priority queue
    //get from top of priority queue
    //call callback function
    //
  }
}
