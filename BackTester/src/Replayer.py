import zmq
import signal
import numpy as np
import talib
import logging
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import sys
sys.path.append('../../ProtoBufMsg/PythonCode')
sys.path.append('../../ProtoBufMsgHub/include')
sys.path.append('../../CedarHelper/include')
import ProtoBufMsg_pb2 as protoMsg
from ProtoBufMsgHub import protoBufHelper as Helper
from ProtoBufMsgHub import protoBufMsgHub as MsgHub
from CedarLogging import Logger

logger = Logger("Replayer").logger

data = []

def onMsg(msg):
  logger.info("onMsg")
  if(msg.type == protoMsg.TYPE_MARKETUPDATE):
    mkt = protoMsg.MarketUpdate()
    mkt = Helper.unWrapMsg(msg, mkt)
    logger.info("nMatch: %d", mkt.latest_price)
    data.append(mkt.latest_price / 10000.0)

msgHub = MsgHub()
msgHub.registerCallBack(onMsg)

Helper.setUpProtoBufMsgHub(msgHub, "../config/default.cfg")

RpRqst = protoMsg.ReplayRequest()
RpRqst.wind_code.extend(["000001.SZ"])
RpRqst.start_date = "20160215"
RpRqst.end_date = "20160217"
RpRqst.day_length = "1"
RpRqst.replay_rate = 10000
str = Helper.wrapMsg(protoMsg.TYPE_REPLAY_REQUEST, RpRqst)
msgHub.pushMsg(str)

def signal_handler(signal, frame):
  macd, macdsignal, macdhist = talib.MACD(np.array(data), fastperiod = 50, slowperiod = 250, signalperiod = 100)



  fig = plt.figure(1)
  fig.suptitle('MACD analysis of 000001.SZ @ 20160215')
  ax = fig.add_subplot(211)
  ax.set_ylabel('price')
  ax.plot(data, 'r', label='Market Data')
  bx = fig.add_subplot(212)
  bx.set_xlabel('tick')
  bx.set_ylabel('val')
  bx.plot(np.zeros(len(data)), 'k')
  bx.plot(macd, 'r', label='MACD')
  bx.plot(macdhist, 'b', label='MACDHIST')
  bx.plot(macdsignal, 'g', label='MACDSIGNAL')
  leg = bx.legend(loc=2,prop={'size':8})
  plt.savefig('Replay-MACD-000001-SZ')
  #print macd
  msgHub.close()
  sys.exit(0)
signal.signal(signal.SIGINT, signal_handler)
signal.pause()
