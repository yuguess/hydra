#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include "CedarLogging.h"
#include "BackTester.h"
#include "CedarJsonConfig.h"

void PrintHelp() {
    std::cout << "\t-d BackTester(default with config file)\n"
              << "\t-b startDate\n"
              << "\t-e endDate\n"
              << "\t-l dayLength\n"
              << "\t-s symbols\n";
}

int main(int argc, char *argv[]) {
  CedarLogging::init("BackTester");
  /* if(argc < 2) { */
  /*   PrintHelp(); */
  /*   exit(0); */
  /* } */

  CedarJsonConfig::getInstance().loadConfigFile("../config/default.cfg");
  std::string startDate, endDate, dayLength; 
  std::string rate = "0";
  std::vector<std::string> symbols;
  bool liveTag = false;
  int opt; 
  while((opt = getopt(argc, argv, "b:dl:e:s:x:L")) != -1) {
    switch(opt) {
      case 'd':
        break;
      case 'b':
        startDate = optarg;
        break;
      case 'e':
        endDate = optarg;
        break;
      case 'l':
        dayLength = optarg;
        break;
      case 's':
        optind--;
        for(; optind < argc && *argv[optind] != '-'; optind++) 
          symbols.push_back(std::string(argv[optind]));
        break;
      case 'x':
        rate = optarg;
        break;
      case 'L':
        liveTag = true;
      default:
        PrintHelp();
        break;
    }
  }

  /* BackTester *bt = new BackTester(startDate, endDate, stoi(dayLength), stoi(rate), symbols); */
  BackTester *bt = new BackTester();
  /* bt->run(); */

  /* LOG(INFO) << "suspend"; */
  CedarHelper::blockSignalAndSuspend();

  /* /1* getchar(); *1/ */
  delete bt;
  /* return 0; */
}
