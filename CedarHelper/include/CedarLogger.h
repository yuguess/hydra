#ifndef CEDAR_LOGGER_H
#define CEDAR_LOGGER_H

#include "easylogging++.h"
#include <fcntl.h>
#ifdef __linux
  #include <unistd.h>
#else
  #include <io.h>
#endif

#include <sys/stat.h>
#include <stdio.h>

class CedarLogger {
public:
  static int init() {
    std::string appName;
    CedarJsonConfig::getInstance().getStringByPath(
      "CedarLogger.AppName", appName);

    el::Configurations defaultConf;

    defaultConf.setToDefault();
    defaultConf.setGlobally(el::ConfigurationType::Format,
      "%levshort%datetime{%M%d %H:%m:%s.%g} %thread %fbase:%line] %msg");

		std::string destDir;
    CedarJsonConfig::getInstance().getStringByPath(
      "CedarLogger.LogDir", destDir);

		defaultConf.set(el::Level::Global, el::ConfigurationType::Filename,
			destDir + appName + "_%datetime{%Y%M%d-%H%m%s.%g}");

    el::Loggers::reconfigureLogger("default", defaultConf);

    std::string filename = el::Loggers::getLogger("default")->
      typedConfigurations()->filename(el::Level::Global);
		filename.erase(0, destDir.length());
		std::string oldSymLink = destDir + appName;
		remove(oldSymLink.c_str());
#ifdef __linux
    int fd = open(destDir.c_str(), O_RDONLY);
		if (symlinkat(filename.c_str(), fd, appName.c_str()) != 0) {
			LOG(FATAL) << appName << " symlink set fail!";
		}
#else
//put win create file link imple here
#endif
    return 0;
  }
};

#endif
