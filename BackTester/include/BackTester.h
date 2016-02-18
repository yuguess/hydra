#ifndef BACKTESTER_H
#define BACKTESTER_H

#include <dirent.h>
#include <ev.h>
#include "ProtoBufMsgHub.h"
#include "MdAdapter.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"


const static int priceLevel = 10;

class Tester {
public:
  Tester()=default;
  virtual ~Tester()=default;
  void spreadCedarMD();
  void updateMD(std::string& msg);
  void newTickAction(std::string& msg);
  virtual void run()=0;

protected:
  static ProtoBufMsgHub msgHub;
  MessageBase msg;
  std::vector<std::string> m_symbols;
};

static void timeout_cb(EV_P_ ev_timer*, int); 

class BackTester : public Tester {
public:
  BackTester(std::string startDate, std::string endDate, int dayLength, int rate, std::vector<std::string> symbols); 
  virtual ~BackTester();

  void run();
  void updateMD();
  void newTickAction();
  void setTimeInterval(int timeInterval);
  void newTick(int sock, int event, void* arg);
  int getFilesToBufQ();

friend void timeout_cb(EV_P_ ev_timer*, int); 

protected:
  static ev_timer timeout_watcher;
  struct ev_loop *loop = EV_DEFAULT;

  std::string m_startDate;
  std::string m_endDate;
  int m_dayLength;
  static int m_rate;
  std::string m_dir;
  struct timeval t;
  static int preTick;
  static int nowTick;

  static MdAdapter *md;

  std::vector<std::string> files;
  std::vector<std::string>::reverse_iterator fileIter;

  int onMsg(MessageBase msg);
};

#endif
