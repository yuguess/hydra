#ifndef RSI_H
#define RSI_H

#include <algorithm>
#include <stdlib.h>
#include "MA.h"

class RSI {
public:
  RSI(int p) : upMA(1.0/p), allMA(1.0/p), preVal(-1), idx(0), period(p) {}

  bool update(double val, double &res) {
    idx++;

    if (preVal != -1) {
      double upDiff = val - preVal;
      res = upMA.update(std::max(upDiff, 0.0)) / allMA.update(fabs(upDiff));
    }

    preVal = val;
    return (idx >= period);
  }

private:
  MA upMA, allMA;
  double preVal;
  int idx, period;
};


#endif
