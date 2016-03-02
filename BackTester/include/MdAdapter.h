#ifndef MDADAPTER_H
#define MDADAPTER_H

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <json/reader.h>
#include <json/writer.h>
#include <unordered_map>
#include <vector>
#include <queue>
#include <ev.h>

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "TDFAPI.h"
#include "CedarHelper.h"

#define WindTypeNum 3
extern std::string WindDataType[]; 

enum {
  MD = 0,
  Idx,
  Trns
};

typedef struct {
  int dd;
  int td;
  int index;
} pqElem;

class pqComp {
public:
  bool operator() (const pqElem a, const pqElem b) {
    if(a.dd == b.dd)
      return a.td >= b.td;
    else 
      return a.dd >= b.dd; 
  }
};

static void timeout_cb(EV_P_ ev_timer*, int);

class MdAdapter {
friend class BackTester;
friend void timeout_cb(EV_P_ ev_timer*, int);
public:
  MdAdapter();
  virtual ~MdAdapter()=default;
  virtual MessageBase getCedarMsg()=0;

protected:
  void mapData(std::string filepath);
  void unmapData();
  void nextTick();
  int getTickIter();
  void inQueue(Json::Value);
  void loadFile(std::string filepath, int type);
  void releaseFile();

  std::unordered_map<std::string, int> iterRecord;

  std::unordered_map<std::string, int> queueMap;
  std::vector<std::queue<Json::Value>> bufQueues;
  std::priority_queue<pqElem, std::vector<pqElem>, pqComp> bufpq;

  void* pMD;
  int totalTick;
  int tickIter;
  long filesize;

  MarketUpdate *elemMsg;
};

class WindMdAdapter : public MdAdapter {
public:
  WindMdAdapter()=default;
  ~WindMdAdapter()=default;
  MessageBase getCedarMsg();
};

#endif
