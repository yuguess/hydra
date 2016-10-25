#!/usr/bin/python
import os
import zmq
import json
import signal
import sys
import logging
import datetime
from datetime import datetime as dt
sys.path.append(os.environ['HOME'] + '/hydra/ProtoBufMsg/PythonCode')
import ProtoBufMsg_pb2 as protoMsg
import getopt

posDirectionDict = {
  "Open" : protoMsg.OPEN_POSITION,
  "Close" : protoMsg.CLOSE_POSITION,
  "CloseToday" : protoMsg.CLOSE_TODAY_POSITION,
  "CloseYesterday" : protoMsg.CLOSE_YESTERDAY_POSITION
}

requestTypeDict = {
  "Limit":protoMsg.TYPE_LIMIT_ORDER_REQUEST,
  "Market":protoMsg.TYPE_MARKET_ORDER_REQUEST,
  "Cancel":protoMsg.TYPE_CANCEL_ORDER_REQUEST,
  "SmartOrder":protoMsg.TYPE_SMART_ORDER_REQUEST,
  "SmallOrder":protoMsg.TYPE_SMALL_ORDER_REQUEST,
  "FirstLevel":protoMsg.TYPE_FIRST_LEVEL_ORDER_REQUEST
}

def idGenerator():
  return datetime.datetime.now().strftime("%H:%M:%S.%f")

def setOrderRequest(orderRequest, req):
  orderRequest.type = requestTypeDict[req["ExecutionType"]]
  orderRequest.response_address = "192.168.0.66:50001"
  orderRequest.account = str(req["Account"])
  orderRequest.id = str(idGenerator())
  orderRequest.code = req["Code"]
  orderRequest.batch_id = req["BatchId"]

  qty = int(req["Qty"])

  if qty >= 0:
    orderRequest.buy_sell = protoMsg.LONG_BUY
    orderRequest.trade_quantity = qty
  else:
    orderRequest.buy_sell = protoMsg.SHORT_SELL
    orderRequest.trade_quantity = -qty
  orderRequest.argument_list = str(req["Args"])

  orderRequest.open_close = posDirectionDict[req["OpenClose"]]

  if (orderRequest.code[0].isdigit()):
    if (int(orderRequest.code[0]) >= 5):
      orderRequest.exchange = protoMsg.SHSE
    else:
      orderRequest.exchange = protoMsg.SZSE

  return

def wrapMsg(msgType, obj):
  msgBase = protoMsg.MessageBase()
  msgBase.type = msgType
  msgBase.msg = obj.SerializeToString()
  return msgBase.SerializeToString()

def helpMsg():
  print "parameter not recognized!"
  print "OrderDispatcher.py -f /path/to/configFile"

if __name__ == '__main__':
  jsonFile = ""
  if (len(sys.argv) < 2):
    helpMsg()
    sys.exit(0)

  try:
    opts, args = getopt.getopt(sys.argv[1:], "f:", ["config="])
  except getopt.GetoptError:
    helpMsg()
    sys.exit(2)

  for opt, arg in opts:
    if opt in ("-f", "--config"):
      jsonFile = arg
    else:
      helpMsg()
      sys.exit(2)

  jsonConfig = json.load(file(jsonFile))
  logPath = jsonConfig["CedarLogger"]["LogDir"]
  logApp = jsonConfig["CedarLogger"]["AppName"]

  logFile = logPath + logApp + "_" + dt.now().strftime('%Y%m%d_%H%M%S') + '.log'
  logger = logging.getLogger(logApp)
  handler = logging.FileHandler(logFile)
  console = logging.StreamHandler()
  fmt = '%(levelname)s_%(asctime)s_%(filename)s:%(lineno)d]%(message)s'
  formatter = logging.Formatter(fmt, "%H:%M:%S")
  handler.setFormatter(formatter)
  logger.addHandler(handler)
  console.setFormatter(formatter)
  logger.addHandler(console)
  logger.setLevel(logging.DEBUG)

  tradeServer = {}

  pullAddr = "tcp://" + jsonConfig["BindAddress"]
  dispatcherServer = jsonConfig["OrderAgent"]["TradeServer"]

  context = zmq.Context()
  orderRecv = context.socket(zmq.PULL)
  orderRecv.bind(pullAddr)

  for serverStat in dispatcherServer:
    targetSocket = context.socket(zmq.PUSH)
    targetSocket.connect("tcp://" + serverStat["address"])
    tradeServer[serverStat["name"]] = targetSocket

  logger.info("OrderDispatcher service online")
  while True:
    actions = orderRecv.recv_json()
    logger.info("<<<" + json.dumps(actions) + ">>>")
    for act in actions:
      if (act["ActionType"] != "BatchTrade"):
        continue

      logger.info(",".join((act["ActionType"], act["User"],
        act["Account"], act["Code"], str(act["Qty"]),
        act["ExecutionType"], act["OpenClose"], act["Args"])));

      req = protoMsg.OrderRequest()
      setOrderRequest(req, act)

      execType = act["ExecutionType"]
      if (execType == "FirstLevel" or execType == "SmartOrder"
          or execType == "SmallOrder"):
        tradeServer["SmartOrderService"].send(
            wrapMsg(protoMsg.TYPE_ORDER_REQUEST, req))
      elif (execType  == "Limit"):
        try:
          req.limit_price = float(act["Args"])
        except ValueError:
          logger.error("limit order wth an invalid price")
          continue
        try:
          tradeServer[act["Account"]].send(
            wrapMsg(protoMsg.TYPE_ORDER_REQUEST, req))
        except KeyError:
          logger.error(act["Account"] + " do not exist")
          continue
