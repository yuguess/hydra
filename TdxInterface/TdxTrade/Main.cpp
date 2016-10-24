#include "stdafx.h"
#include "TdxManager.h"
#include "CedarJsonConfig.h"
#include <fstream>
#include <signal.h>
#ifdef __linux
#include "IncludeOnlyInMain.h"
#endif



BOOL WINAPI ConsoleHandler(DWORD);

int main(int argc, char *argv[]) {
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE)) {
		fprintf(stderr, "Unable to install handler!\n");
		return EXIT_FAILURE;
	}

	CedarHelper::cedarAppInit(argc, argv);

	//CedarJsonConfig::getInstance().loadConfigFile("C:\\TdxInterface\\Debug\\TdxTrade.json");
	
	TdxManager tdm;
	tdm.initialize();
	
	if (tdm.logon() == -1) {
		LOG(FATAL) << "Login Fail, Please check configuration and try again!" << endl;
		return 0;
	}
	
	if (tdm.startService() != 0) {
		LOG(INFO) << "Error Happened in Starting Service, please check." << endl;
	}
	
	getchar();
	return 0;
}

BOOL WINAPI ConsoleHandler(DWORD dwType) {
	switch (dwType) {
	case CTRL_C_EVENT:
		exit(-1);
		break;
	case CTRL_BREAK_EVENT:
		printf("break\n");
		break;
	default:
		printf("Some other event\n");
	}
	return TRUE;
}