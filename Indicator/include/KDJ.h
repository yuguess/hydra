#ifndef KDJ_H
#define KDJ_H

#include "MA.h"
#include <list>
#include <algorithm>

class KD {
public:
  KD(int _kPeriod, int _fastk, int _slowk) :
    kMA(1.0 / _fastk), dMA(1.0 / _slowk),
    kPeriod(_kPeriod), fastk(_fastk), slowk(_slowk)  {}

  bool update(double high, double low, double close, double &k, double &d) {
    highBuf.push_back(high);
    lowBuf.push_back(low);

    if (highBuf.size() == kPeriod) {
      return calcKD(close, k, d);
    } else if (highBuf.size() > kPeriod) {
      highBuf.pop_front();
      lowBuf.pop_front();
      return calcKD(close, k, d);
    }

    return false;
  }

private:
  bool calcKD(double close, double &k, double &d) {
    double minLow = *std::min_element(std::begin(lowBuf), std::end(lowBuf));
    double maxHigh = *std::max_element(std::begin(highBuf), std::end(highBuf));

    if (maxHigh - minLow < 1e-8)
      return false;

    double rsv = (close - minLow) / (maxHigh - minLow);

    k = kMA.update(rsv);
    d = dMA.update(k);
    return true;
  }

  MA  kMA, dMA;
  std::list<double> highBuf, lowBuf;
  unsigned kPeriod, fastk, slowk;
};
#endif
