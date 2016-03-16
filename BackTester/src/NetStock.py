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
PriceRec = {}
buyRec = {}
for code in codes:
  PriceRec[code.split('.')[0]] = []
  buyRec[code.split('.')[0]] = 0

LOR = 100  # length limit of record
TW = 100000 # time window
TH = 1000   # threshold of price variance
STH = 60 # threshold of price variance for related stocks
start = '2015-12-14'
end = '2016-03-14'
industry = '银行'

writer = open("/var/log/td-agent/netStock.log", 'a')

calCorre = CalCorre()
logger.info("loading pearson correlation values...")
result = calCorre.CorreOfClosePrice(start, end, industry)
logger.info("load completed")

def checkRelatedStocks(tstock):
  # logger.info("check related stocks")
  relatedStocks = result[tstock].order(ascending=False).index
  for rstock in relatedStocks.values[1:4]:
    tag = True
    count = 0
    mktUpdt = 0
    for item in reversed(PriceRec[rstock]):
      if count == 0:
        count += 1
        mktUpdt = item
        continue
      # logger.info("recent time %s, last time %s ", mktUpdt.exchange_timestamp, item.exchange_timestamp)
      if int(mktUpdt.exchange_timestamp) -  int(item.exchange_timestamp) > TW:
        break;
      # logger.info("recent price %d, last price %d ", mktUpdt.latest_price, item.latest_price)
      if abs(mktUpdt.latest_price - item.latest_price) > STH:
        tag = False
        logger.info("not interested!")
        break;
      count += 1
    if tag:
      logger.info("buy %s, @price: %d", rstock, mktUpdt.latest_price)
      buyRec[rstock] += 1

def dealWithOneTick(mktUpdt):
  # logger.info("dealWithOneTick %d", mktUpdt.latest_price)

  length = len(PriceRec[mktUpdt.code])
  count = 0
  for item in reversed(PriceRec[mktUpdt.code]):
    if count >= LOR:
      break;
    if int(mktUpdt.exchange_timestamp) -  int(item.exchange_timestamp) > TW:
      break
    else:
      diff = mktUpdt.latest_price - item.latest_price
      if abs(diff) > TH:
        tag = "up"
        if diff > 0:
          logger.info("code: %s, up signal: %d %d", mktUpdt.code, mktUpdt.latest_price, item.latest_price)
        else:
          logger.info("code: %s, down signal: %d %d", mktUpdt.code, mktUpdt.latest_price, item.latest_price)
          tag = "down"
        checkRelatedStocks(mktUpdt.code)
        jsonObj = {"code":mktUpdt.code, "signal":tag, "price":mktUpdt.latest_price, "compared price":item.latest_price}
        with open("/var/log/td-agent/netStock.log", 'w') as f:
          json.dump(jsonObj, f)
          f.write('\n')
        break
      count += 1

  PriceRec[mktUpdt.code].append(mktUpdt)

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
