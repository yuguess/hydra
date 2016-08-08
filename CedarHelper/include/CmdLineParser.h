#ifndef CMD_LINE_PARSER_H
#define CMD_LINE_PARSER_H

#include <iostream>
#include <unistd.h>

class CmdLineParser {
public:
  static std::string getConfigPathFromCmd(int argc, char *argv[]) {
    int opt;
    if ((opt = getopt(argc, argv, "f:h")) == EOF || opt == 'h') {
      printHelp();
      exit(1);
    } else {
      switch(opt) {
        case 'f':
          return optarg;
        default:
          printHelp();
          exit(1);
      }
    }
  }

private:
  static int printHelp() {
    std::cout << "\t-f path to config file\n"
      << "\t-h print help message\n";
    return 0;
  }
};

#endif
