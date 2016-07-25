#include "StockProcessor.h"
#include "ProtoBufMsgHub.h"
#include "CedarJsonConfig.h"

const double StockProcessor::priceAnomalyThreshold = 0.005;

inline double weightedPrice(MarketUpdate &mkdt) {
  double vol = mkdt.ask_volume(0) + mkdt.bid_volume(0);
  if (vol == 0) {
    LOG(WARNING) << "Recv an invalid markupdate with ask and bid volume be 0";
    return 0.0;
  }

  double askWeight = static_cast<double>(mkdt.bid_volume(0)) / vol;
  double bidWeight = static_cast<double>(mkdt.ask_volume(0)) / vol;

  double midPrice = mkdt.ask_price(0) * askWeight + 
    mkdt.bid_price(0) * bidWeight;
  return midPrice;
}

inline double timeDiffInSecond(TimeStamp now, TimeStamp past) {
  std::chrono::duration<double> span = 
    std::chrono::duration_cast<std::chrono::duration<double>>(past - now);

  return span.count();
}

int StockProcessor::init(std::string argCode) {
  code = argCode;
  return 0;
}

int StockProcessor::onMarketUpdate(MarketUpdate mkdt) {
  priceAnomaly(mkdt);
  //volumeAnomaly();
  return 0;
}

int StockProcessor::priceAnomaly(MarketUpdate &mkdt) {
  DataBundle data;
  data.ts = std::chrono::system_clock::now();
  data.mkdt = mkdt;

  dataBuf.push_back(data);
  if (dataBuf.size() < initBufSize)
    return 0;

  DataBundle curData = dataBuf[0];
  DataBundle preData = dataBuf[1];

  double tsDiff =  timeDiffInSecond(curData.ts, preData.ts);
  double curPrice = weightedPrice(curData.mkdt);
  double prePrice = weightedPrice(preData.mkdt);
  double prctDiff = (curPrice - prePrice) * 100 / curPrice;

  if (tsDiff > 1e-6) {
    double prcAcc = prctDiff / tsDiff;

    //LOG(INFO) << "prctDiff " << prctDiff << " tsDiff " << tsDiff;

    if (prcAcc > priceAnomalyThreshold) {
      LOG(INFO) << "Ticker " << code << " speed increase change alert";
    } else if (prcAcc < -priceAnomalyThreshold) {
      LOG(INFO) << "Ticker " << code << " speed decrease change alert";
    }
  }

  return 0;
}
