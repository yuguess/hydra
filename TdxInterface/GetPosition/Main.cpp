#include "stdafx.h"
#include "TdxApi.h"
#include "stdafx.h"
#include <iostream>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "boost/date_time/local_time_adjustor.hpp"
#include "boost/date_time/c_local_time_adjustor.hpp"
#define RESULT_SIZE 1024*1024
#define ERROR_INFO_SIZE 512*512
#define QUE_CODE_CHECK_ORDER 0
using namespace std;

int main() {
	fstream oo("C:\\test\\ZX_data");
	if (!oo) {
		printf("open error\n");
	}
	CTdxApi TdxApi;
	TdxApi.OpenTdx();
	char *result = new char[RESULT_SIZE];
	char *errInfo = new char[ERROR_INFO_SIZE];
	for (int i = 0; i < RESULT_SIZE; i++) {
		result[i] = '\0';
	}

	//int client = TdxApi.Logon("rzjy1.gtja.com", 7708, "6.00", 35, 8, "1869988", "1869988", "112233", "112233", errInfo);

	//int client = TdxApi.Logon("mock.tdx.com.cn", 7708, "8.0", 9000, 8, "yuguess@gmail.com", "f001001001005128", "", "3314145", errInfo);
	//int client = TdxApi.Logon("124.74.242.150", 443, "2.53", 127, 8, "201500061185", "201500061185", "147369", "147369", errInfo);
	//int client = TdxApi.Logon("61.152.107.139", 7708, "8.0", 9000, 8, "yuguess@gmail.com", "f001001001005128", "3314145", "", errInfo);
	//int client = TdxApi.Logon("180.153.18.173", 7708, "6.45", 1,8, "8700000608", "8700000608", "100888", "100888", errInfo);
	//int client = TdxApi.Logon("140.207.225.74", 7708, "7.02", 1, 8, "31279483", "31279483", "571383", "571383", errInfo);      //GJ
	//int client = TdxApi.Logon("124.74.242.150", 443, "2.53", 127, 8, "201500061185", "201500061185", "147369", "147369", errInfo);
	int client = TdxApi.Logon("123.232.108.84", 7700, "1.19", 0, 0, "909700013569", "909700013569", "159357", "159357", errInfo);

	if (client == -1) {
		printf("log on error\n");
		cout << errInfo;
	}
	else {
		printf("log on, client id:%d\n", client);
		cout << "getting position"<<endl;
		TdxApi.QueryData(client, 2, result, errInfo);

		cout << "result:" << result << endl;
		cout << "err:" << errInfo << endl;
		cout << "test:" << (errInfo[0] == '\0') << endl;
	}

	//cout << "result is:" << result;
	oo << result;
	oo.close();
	TdxApi.CloseTdx();
	getchar();
	return 0;
}