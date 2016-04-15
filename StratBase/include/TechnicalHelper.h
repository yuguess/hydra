#ifndef TECHNICAL_HELPER_H
#define TECHNICAL_HELPER_H

#include "CedarLogging.h"

enum CrossStatus {
  NO_CROSS = 0,
  UP_CROSS, 
  DOWN_CROSS
};

class TechnicalHelper {

public:
  static CrossStatus checkCross(double firBeg, double firEnd,
      double secBeg, double secEnd) {
    if (firBeg == secBeg) {
      LOG(ERROR) << "checkCross error, begin point overlap"; 
    }

    if (firBeg < secBeg && firEnd >= secEnd) {
      return UP_CROSS;
    } else if (firBeg > secBeg && firEnd <= secEnd) {
      return DOWN_CROSS;
    } else {
      return NO_CROSS;
    }
  }
};

#endif
