# -*- coding: utf-8 -*-
import talib
from scipy.stats.stats import pearsonr
import signal
import json
from Queue import Queue
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
from CalCorre import CalCorre

logger = Logger(os.path.basename(__file__)).logger
codes = ["000001.SZ", "002142.SZ", "601939.SH", "600000.SH", "601166.SH", "601818.SH", "601998.SH", "601988.SH", "601398.SH", "601328.SH", "601288.SH", "601169.SH", "601009.SH", "600036.SH", "600016.SH", "600015.SH"]
priceRec = {}  # record mktUpdt of each stocks
buyRec = {}    # record buy times of each stocks
recentRec = {} # record recent tick price
preRec = {}    # record 10s earlier tick price
for code in codes:
  priceRec[code.split('.')[0]] = []
  buyRec[code.split('.')[0]] = 0
  recentRec[code.split('.')[0]] = []
  preRec[code.split('.')[0]] = []

LOR = 100   # length limit of record
TW = 100000 # time window
TH = 1000   # threshold of price variance
STH = 60    # threshold of price variance for related stocks
recLen = 10 # length of recentRec and preRec
start = '2015-12-14'
end = '2016-03-14'
industry = '银行'

calCorre = CalCorre()
logger.info("loading pearson correlation values...")
result = calCorre.CorreOfClosePrice(start, end, industry)
logger.info("load completed")

def checkRelatedStocks(tstock):
  logger.info("check related stocks")
  relatedStocks = result[tstock].order(ascending=False).index
  for rstock in relatedStocks.values[1:4]:
    if len(preRec[rstock]) == 0:
      continue
    if (sum(recentRec[rstock]) / len(recentRec[rstock])) / (sum(preRec[rstock]) / recLen) < 0.03:
      logger.info("buy %s @ %f", rstock, priceRec[rstock][-1])
      buyRec[rstock] += 1
    else:
      logger.info("not interested")

def dealWithOneTick(mktUpdt):
  logger.info("dealWithOneTick %f", mktUpdt.latest_price / 10000.0)

  length = len(priceRec[mktUpdt.code])

  code = mktUpdt.code
  recentRec[code].append(float(mktUpdt.latest_price) / 10000.0)
  priceRec[code].append(float(mktUpdt.latest_price) / 10000.0)

  if len(recentRec[code]) >= recLen:
    if len(preRec[code]) >= recLen:
      if (sum(recentRec[code]) / recLen) / (sum(preRec[code]) / recLen) > 0.04:
        checkRelatedStocks(code)
    preRec[code] = recentRec[code]

def onMsg(msg):
  mktUpdt = protoMsg.MarketUpdate()
  mktUpdt = Helper.unWrapMsg(msg, mktUpdt)
  if mktUpdt.latest_price <= 0:
    return
  # logger.info("code: %s day: %d time: %s latest_price: %d", mktUpdt.code, mktUpdt.action_day, mktUpdt.exchange_timestamp, mktUpdt.latest_price)
  dealWithOneTick(mktUpdt)

msgHub = MsgHub()
msgHub.registerCallBack(onMsg)
Helper.setUpProtoBufMsgHub(msgHub, "../config/default.cfg")

dtRqst = DataRequest()
dtRqst.code.extend(codes)
# dtRqst.exchange = "SZ"
str = Helper.wrapMsg(protoMsg.TYPE_DATAREQUEST, dtRqst)
msgHub.pushMsg(str)

def signal_handler(signal, frame):
  print("signal_hanlder called")
  print buyRec

  msgHub.close()
  sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)
signal.pause()
