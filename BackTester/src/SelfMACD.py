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
shortLen = 50
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
  logger.info("code: %s day: %d time: %s latest_price: %d", mktUpdt.code, mktUpdt.action_day, mktUpdt.exchange_timestamp, mktUpdt.latest_price)
  dealWithOneTick(mktUpdt)

msgHub = MsgHub()
msgHub.registerCallBack(onMsg)
Helper.setUpProtoBufMsgHub(msgHub, "../config/default.cfg")

dtRqst = DataRequest()
dtRqst.code.extend(["000001"])
dtRqst.exchange = "SZ"
str = Helper.wrapMsg(protoMsg.TYPE_DATAREQUEST, dtRqst)
msgHub.pushMsg(str)


def drawPics():
  plt.figure(1)

  ax = plt.subplot(311)
  ax.plot(np.array(data), color = 'k', label="raw data")
  for elem in buySignal:
    ax.axvline(elem, color='r')
  for elem in sellSignal:
    ax.axvline(elem, color='b')
  ax.legend(loc=2, prop={'size':7})
  ax.set_ylabel('prices')
  plt.title('selfMACD-000001SZ')

  bx = plt.subplot(312)
  bx.plot(np.array(shortList), 'b', np.array(longList), 'g')
  bx.legend(('short AVG(50 ticks)', 'long AVG(250 ticks)'), loc=2, prop={'size':7})
  bx.set_ylabel('avg prices')

  cx = plt.subplot(313)
  cx.axhline(0, color='k')
  cx.plot(np.array(result), 'r', label="MACD")
  cx.legend(loc=2, prop={'size':7})
  cx.set_ylabel('MACD value')
  cx.set_xlabel('Ticks')

  plt.savefig('selfMACD-000001SZ')

  print(buySignal)
  print(sellSignal)


def calearning():
  totalearning = 0
  for tick in buySignal:
    totalearning -= data[tick]
    logger.info("tick %d buy @ %f update earning: %f", tick, data[tick], totalearning)
  for tick in sellSignal:
    totalearning += data[tick]
    logger.info("tick %d sell @ %f update earning: %f", tick, data[tick], totalearning)
  loafearning = data[len(data) - 1] - data[0]
  logger.info("totalearning: %f vs loafearning: %f", totalearning, loafearning)


def signal_handler(signal, frame):
  print("signal_hanlder called")

  drawPics()
  calearning()

  msgHub.close()
  sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)
signal.pause()
