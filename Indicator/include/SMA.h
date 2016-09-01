#ifndef SMA_H
#define SMA_H

#include <list>

class SMA {

public:
  SMA(int p) : period(p), preSum(0.0) {
    buf.resize(period, 0);
  }

  double update(double val) {
    preSum += (val - buf.front()) / period;
    buf.pop_front();
    buf.push_back(val);
    return preSum;
  }

private:
  int period;
  double preSum;
  std::list<double> buf;
};

#endif
