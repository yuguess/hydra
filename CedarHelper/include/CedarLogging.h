#ifndef CEDAR_LOGGING_H
#define CEDAR_LOGGING_H

#include "easylogging++.h"

#define ELPP_NO_DEFAULT_LOG_FILE
INITIALIZE_EASYLOGGINGPP

class CedarLogging {
public:

  static int init(std::string program) {
    el::Configurations defaultConf;
    defaultConf.setToDefault();

    defaultConf.setGlobally(
        el::ConfigurationType::Format, "%levshort%datetime{%M%d %H:%m:%s.%g} %thread %fbase:%line] %msg");

    std::string destDir = "./log/";
    defaultConf.set(el::Level::Info,    
        el::ConfigurationType::Filename, destDir + program + "_INFO_%datetime{%Y%M%d-%H%m%s.%g}");
    defaultConf.set(el::Level::Warning,    
        el::ConfigurationType::Filename, destDir + program + "_WARNING_%datetime{%Y%M%d-%H%m%s.%g}");
    defaultConf.set(el::Level::Error,    
        el::ConfigurationType::Filename, destDir + program + "_ERROR_%datetime{%Y%M%d-%H%m%s.%g}");
    defaultConf.set(el::Level::Fatal,    
        el::ConfigurationType::Filename, destDir + program + "_FATAL_%datetime{%Y%M%d-%H%m%s.%g}");

    el::Loggers::reconfigureLogger("default", defaultConf);

    return 0;
  }
};

#endif
