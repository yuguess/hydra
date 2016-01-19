#ifndef CEDAR_LOGGING_H
#define CEDAR_LOGGING_H

#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

class CedarLogging {
public:

  static int init() {
    return 0;
  }
};

#endif
