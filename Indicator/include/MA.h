#ifndef MA_H
#define MA_H

class MA {
public:
  MA(double newValWeight) : weight(newValWeight), preVal(0.0) {};

  double update(double val) {
    preVal = (preVal * (1 - weight) + val * weight);
    return preVal;
  }

private:
  double weight;
  double preVal;
};

#endif
