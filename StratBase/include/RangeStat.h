#ifndef RANGE_STAT_H
#define RANGE_STAT_H

class RangeStat {
public:
  int marketUpdate() {

  }

  int registerCallback() {

  }

  double getOpen() {

  }

  double getHigh() {

  }

  double getLow() {

  }

  double getClose() {

  }

private:
  int open, high, low, close;
  ProtoBufMsgHub::MsgCallback msgCallback;
};

#endif
