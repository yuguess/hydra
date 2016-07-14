# module for msgHub in python
import signal
import time
import threading
import json
import zmq
import logging
import sys
sys.path.append('../../ProtoBufMsg/PythonCode')
sys.path.append('../../CedarHelper/include')
import ProtoBufMsg_pb2 as protoMsg
from CedarLogging import Logger

logger = Logger("ProgoBufMsgHub").logger

class protoBufMsgHub:
  def __init__(self):
    self.closeflag = True
    self.poller = zmq.Poller()

  def __del__(self):
    self.closeflag = False

  def init(self, push, sub):
    self.pushAddr = "tcp://127.0.0.1:" + push
    self.subAddr = "tcp://127.0.0.1:" + sub

    context_push = zmq.Context()
    self.socket_push = context_push.socket(zmq.PUSH)
    self.socket_push.connect(self.pushAddr)

    context_sub = zmq.Context()
    self.socket_sub = context_sub.socket(zmq.SUB)
    self.socket_sub.connect(self.subAddr)
    self.socket_sub.setsockopt(zmq.SUBSCRIBE, "")
    self.poller.register(self.socket_sub, zmq.POLLIN)

    try:
      self.t = threading.Thread(target=self.poll)
      #self.t.daemon = True
      self.t.start()
    except:
      logger.error("unable to start thread")

  def pushMsg(self, str):
    self.socket_push.send(str)
    logger.info("push msg: %s", str)

  def poll(self):
    while self.closeflag:
      socks = dict(self.poller.poll(100))
      if self.socket_sub in socks and socks[self.socket_sub] == zmq.POLLIN:
        self.recvMsg()

  def recvMsg(self):
    try:
      msg = self.socket_sub.recv()
    except:
      return 0

    msg = self.removeChannelHeader(msg)
    msgbase = protoMsg.MessageBase()
    try:
      msgbase.ParseFromString(msg)
    except:
      return 0
    self.msgCallBack(msgbase)

  def close(self):
    self.closeflag = False
    logger.info("msgHub closeing")

  def removeChannelHeader(self, subMsg):
    str = subMsg.split('|')[0]
    if(str is None or str == subMsg):
      logger.error("recv invalid msg")
      return -1
    return subMsg.split("|")[1]

  def registerCallBack(self, func):
    self.msgCallBack = func

class protoBufHelper:
  @staticmethod
  def wrapMsg(msgType, obj):
    msgBase = protoMsg.MessageBase()
    msgBase.type = msgType
    msgBase.msg = obj.SerializeToString()

    return msgBase.SerializeToString()

  @staticmethod
  def unWrapMsg(msgBase, obj):
    obj.ParseFromString(msgBase.msg)
    return obj

  @staticmethod
  def setUpProtoBufMsgHub(msgHub, cfg):
    with open(cfg) as data_file:
      data = json.load(data_file)

    PushPort = data["PythonMsgHub"]["PushPort"]
    SubPort = data["PythonMsgHub"]["SubPort"]
    msgHub.init(PushPort, SubPort)
