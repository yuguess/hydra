#ifndef G_LOGGER
#define G_LOGGER

#ifdef __linux
#include "CedarLogger.h"
#else
#include "glog/logging.h"
#endif

class GLogger {
public:
	static int initGlog(std::string program, int severity = google::GLOG_INFO) {
		//system("mkdir -p log");
		google::InitGoogleLogging(program.c_str());

		FLAGS_log_dir = "./log";
		google::SetLogDestination(google::GLOG_INFO, (program + "_INFO_").c_str());
		google::SetLogDestination(google::GLOG_WARNING, (program + "_WARNING_").c_str());
		google::SetLogDestination(google::GLOG_ERROR, (program + "_ERROR_").c_str());
		google::SetLogDestination(google::GLOG_FATAL, (program + "_FATAL_").c_str());
		FLAGS_stderrthreshold = severity;

		FLAGS_colorlogtostderr = true;
		FLAGS_logbufsecs = 0;
		FLAGS_max_log_size = 100;
		FLAGS_stop_logging_if_full_disk = true;

		LOG(INFO) << "glog init finish";

		return 0;
	}

};

#endif