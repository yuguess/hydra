#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

#include "time.h"
#include <boost\algorithm\string.hpp>

#include "Parameters.h"
using namespace std;

//split single line
static vector<string> split_data_own(char in[]) {
	long long i;
	long long length;
	int pos = 0;

	vector<string> result;
	string inData = in;
	string data = "";
	length = inData.size();
	while (true) {
		data = "";
		while (inData[pos] != '\n' && inData[pos] != '\0') {
			data += inData[pos];
			pos++;

			if (pos >= length) {
				break;
			}
		}

		result.push_back(data);
		if (pos >= length - 1) break;

		pos = pos + 1;
	}

	return result;
}

static vector<string> split_line_own(char in[]) {
	long long i;
	long long length;
	int pos = 0;

	vector<string> result;
	string inData = in;
	string data = "";
	length = inData.size();
	while (true) {
		data = "";
		while (inData[pos] != '\t' && inData[pos] != '\0') {
			data += inData[pos];
			pos++;
			//cout << data << endl;
			if (pos >= length) {
				//cout << "break1";
				break;
			}
		}

		//data += '\0';

		result.push_back(data);
		if (pos >= length - 1) break;

		pos = pos + 1;
	}

	return result;
}

static unordered_map<int, string> GetHolder(vector<string> in) {

	unordered_map<int, string> map;
	int count = 0;

	vector<string>::iterator iter1 = in.begin(), iter2 = in.end();
	for (iter1; iter1 != iter2; iter1++) {
		cout << "num: " << count << *iter1 << endl;
		count++;
	}
	
	if (count != 21) {
		cout << "Information Error." << endl;
		return map;
	}

	map[HU_HOLDER_CODE] = in.at(7);
	map[SHEN_HOLDER_CODE] = in.at(14);

	return map;
}

static string getIDfromLine(string in) {
	vector<string> InLineData;
	boost::split(InLineData, in, boost::is_any_of("\t\0"), boost::token_compress_on);

	return InLineData[9];
}


