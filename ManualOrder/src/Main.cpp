#include "ManualOrder.h"
#include "CedarHelper.h"
#include "CedarLogger.h"
#include "IncludeOnlyInMain.h"

int main(int argc, char *argv[]) {
  CedarHelper::cedarAppInit(argc, argv);

  ManualOrder manualOrder;
  manualOrder.run();
  //SimpleTester sTester;
  //sTester.autorun();
  return 0;
}
