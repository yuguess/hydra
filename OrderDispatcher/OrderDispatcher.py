import os
import zmq
import json
import signal
import sys
import logging
import datetime
from datetime import datetime as dt
sys.path.append('/home/yuguess/hydra/ProtoBufMsg/PythonCode')
import ProtoBufMsg_pb2 as protoMsg

########config goes below###############
jsonFile = "./config/OrderDispatcher.json"
localAddr = "192.168.0.66"
idCount = 0
#######################################

posDirectionDict = {
  "Open" : protoMsg.OPEN_POSITION,
  "Close" : protoMsg.CLOSE_POSITION,
  "Close_Today" : protoMsg.CLOSE_TODAY_POSITION,
  "Close_Yesterday" : protoMsg.CLOSE_YESTERDAY_POSITION
}

requestTypeDict = {
  "Limit":protoMsg.TYPE_LIMIT_ORDER_REQUEST,
  "Market":protoMsg.TYPE_MARKET_ORDER_REQUEST,
  "Cancel":protoMsg.TYPE_CANCEL_ORDER_REQUEST,
  "SmartOrder":protoMsg.TYPE_SMART_ORDER_REQUEST,
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

  if req["Qty"] >= 0:
    orderRequest.buy_sell = protoMsg.LONG_BUY
    orderRequest.trade_quantity = req["Qty"]
  else:
    orderRequest.buy_sell = protoMsg.SHORT_SELL
    orderRequest.trade_quantity = -req["Qty"]
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

if __name__ == '__main__':
  logFile = "logs/" + dt.now().strftime('%Y%m%d_%H%M%S') + '.log'
  logger = logging.getLogger('OrderDispatcher')
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

  jsonConfig = json.load(file(jsonFile))
  pullAddr = "tcp://" + localAddr + ":" + jsonConfig["MsgHub"]["PullPort"]
  dispatcherServer = jsonConfig["TradeServer"]

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
    for act in actions:
      if (act["ActionType"] != "BatchTrade"):
        continue

      logger.info(",".join((act["ActionType"], act["User"],
        act["Account"], act["Code"], str(act["Qty"]),
        act["ExecutionType"], act["OpenClose"], act["Args"])));

      req = protoMsg.OrderRequest()
      setOrderRequest(req, act)

      execType = act["ExecutionType"]
      if (execType == "FirstLevel" or execType == "SmartOrder"):
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
