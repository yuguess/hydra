import talib
import signal
import Queue
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os
import sys
sys.path.append('../../ProtoBufMsg/PythonCode/')
sys.path.append('../../ProtoBufMsgHub/include/')
sys.path.append('../../CedarHelper/include/')
import ProtoBufMsg_pb2 as protoMsg
from ProtoBufMsg_pb2 import DataRequest
from ProtoBufMsgHub import protoBufMsgHub as MsgHub
from ProtoBufMsgHub import protoBufHelper as Helper
from CedarLogging import Logger

logger = Logger(os.path.basename(__file__)).logger

data = []
longLen = 250
result = [None] * longLen
shortSum = 0
longSum = 0
shortq = Queue.Queue(maxsize = 50)
longq = Queue.Queue(maxsize = 250)
shortList = [None] * longLen
longList = [None] * longLen
buySignal = []
sellSignal = []
tickNum = 0

def dealWithOneTick(mktUpdt):
  global shortSum, longSum, tickNum
  nowPrice = mktUpdt.latest_price / 10000.0
  if nowPrice <= 0:
    return
  data.append(nowPrice)
  if not shortq.full():
    shortq.put(nowPrice)
    shortSum += nowPrice
  else:
    shortSum = shortSum - shortq.get() + nowPrice
    shortq.put(nowPrice)

  if not longq.full():
    longq.put(nowPrice)
    longSum += nowPrice
  else:
    longSum = longSum - longq.get() + nowPrice
    longq.put(nowPrice)

  if shortq.full() and longq.full():
    shortAvg = float(shortSum) / shortLen
    longAvg = float(longSum) / longLen
    shortList.append(shortAvg)
    longList.append(longAvg)
    result.append(shortAvg - longAvg)
    if len(result) > longLen+10 and result[tickNum] * result[tickNum - 1] <= 0:
      if result[tickNum] > 0 or result[tickNum - 1] < 0:
        buySignal.append(tickNum)
        logger.info("buy @ price %f tick %d", nowPrice, tickNum)
      if result[tickNum] < 0 or result[tickNum - 1] > 0:
        sellSignal.append(tickNum)
        logger.info("sell @ price %f tick %d", nowPrice, tickNum)
    # logger.info("shortAvg %f longAvg %f", shortAvg, longAvg)

  tickNum += 1

def onMsg(msg):
  mktUpdt = protoMsg.MarketUpdate()
  mktUpdt = Helper.unWrapMsg(msg, mktUpdt)
  # logger.info("latest_price: %d", nowPrice)
  dealWithOneTick(mktUpdt)

msgHub = MsgHub()
msgHub.registerCallBack(onMsg)
Helper.setUpProtoBufMsgHub(msgHub, "../config/default.cfg")

dtRqst = DataRequest()
dtRqst.code.extend(["000001"])
dtRqst.exchange = "SZ"
str = Helper.wrapMsg(protoMsg.TYPE_DATAREQUEST, dtRqst)
msgHub.pushMsg(str)

def signal_handler(signal, frame):
  print("signal_hanlder called")

  plt.figure(1)

  ax = plt.subplot(311)
  ax.plot(np.array(data), color = 'k', label="raw data")
  ax.legend(loc=2, prop={'size':7})
  ax.set_ylabel('prices')
  plt.title('selfMACD-000001SZ')

  bx = plt.subplot(312)
  bx.plot(np.array(shortList), 'b', np.array(longList), 'g')
  bx.legend(('short AVG(50 ticks)', 'long AVG(250 ticks)'), loc=2, prop={'size':7})
  bx.set_ylabel('avg prices')

  cx = plt.subplot(313)
  cx.plot(np.zeros(len(data)), 'k')
  cx.plot(np.array(result), 'r', label="MACD")
  cx.legend(loc=2, prop={'size':7})
  cx.set_ylabel('MACD value')
  cx.set_xlabel('Ticks')

  plt.savefig('selfMACD-000001SZ')

  msgHub.close()
  sys.exit(0)

shortLen = 50

signal.signal(signal.SIGINT, signal_handler)
signal.pause()
