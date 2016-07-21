import os
import zmq
import json
import signal
import sys
import logging
from datetime import datetime as dt

sys.path.append('/home/yuguess/hydra/ProtoBufMsg/PythonCode')
import ProtoBufMsg_pb2 as protoMsg

jsonFile = "config/OrderDispatcher.json"

idCount = 0

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
    "Smart":protoMsg.TYPE_SMART_ORDER_REQUEST,
    "FirstLevel":protoMsg.TYPE_FIRST_LEVEL_ORDER_REQUEST
}

def idGenerator():
    global idCount
    idCount = idCount + 1
    return idCount

def setOrderRequest(orderRequest, act):
    orderRequest.type = requestTypeDict[act["ExecutionType"]]
    orderRequest.response_address = "192.168.0.66:50000"
    orderRequest.account = str(act["Account"])
    orderRequest.id = str(idGenerator())
    orderRequest.code = act["Ticker"]
    orderRequest.trade_quantity = act["Qty"]
    if act["Qty"] >= 0:
        orderRequest.buy_sell = protoMsg.LONG_BUY
    else:
        orderRequest.buy_sell = protoMsg.SHORT_SELL
    orderRequest.argument_list = str(act["Args"])
    orderRequest.open_close = posDirectionDict[act["OpenClose"]]
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
    formatter = logging.Formatter('%(levelname)s - %(asctime)s - %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)
    logger.setLevel(logging.DEBUG)

    tradeServer = {}

    jsonConfig = json.load(file(jsonFile))
    pull_Addr = "tcp://192.168.0.66:" + jsonConfig["MsgHub"]["PullPort"]
    dispatcherServer = jsonConfig["TradeServer"]

    context = zmq.Context()
    orderRecv = context.socket(zmq.PULL)
    orderRecv.bind(pull_Addr)

    for serverStat in dispatcherServer:
        targetSocket = context.socket(zmq.PUSH)
        targetSocket.connect("tcp://" + serverStat["address"])
        tradeServer[serverStat["name"]] = targetSocket

    logger.info("OrderDispatcher service online")
    while True:
        actions = orderRecv.recv_json()
        for act in actions:
            if (act["ActionType"] == "BatchTrade"):
                logger.info(",".join((act["ActionType"], act["User"],
                    act["Account"], act["Ticker"], str(act["Qty"]),
                    act["ExecutionType"], act["OpenClose"], str(act["Args"]))));

                orderRequest = protoMsg.OrderRequest()
                setOrderRequest(orderRequest, act)
                msg = wrapMsg(protoMsg.TYPE_ORDER_REQUEST, orderRequest)
                if msg != None:
                    tradeServer[act["Account"]].send(msg)
