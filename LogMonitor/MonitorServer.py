#!/usr/bin/python
import os.path
import time
import signal
import sys
import re
import pandas as pd
import ast

import tornado.web
import tornado.ioloop
import tornado.websocket
import MonitorData as mo
from threading import Thread

#######config ###############
#logFile = "/home/infra/MonitorLog/2016-08-22"
logFile = "/home/infra/MonitorLog/" + time.strftime("%Y-%m-%d")
#logFile = "/home/mwan/LogMonitor/log"
#logFile_app = "/home/infra/MonitorLog/ALGO_"+time.strftime("%Y-%m-%d")
#logFile = "/home/mwan/LogMonitor/log1"
listenPort = "8213"
#############################
print logFile

init_table_finish_flag = 0
app_line_buf = []
mddata = mo.MonitorData();

try:
  fp = open(logFile, 'r')
except:
  fp = open(logFile, 'w')
fp.close()
fp = open(logFile, 'r')

order_position = {}
order_num_count = 0
algo_position = {}
algo_num_count = 0
batch_position = {}
batch_num_count = 0

algo_id_map = {}
batch_id_map = {}

batch_head = ["batch_id", "trade_quantity", "quantity"]
algo_head = ["algo_id", "code", "buy/sell", "trade_quantity", "quantity"]
order_head = ["status", "code", "price", "trade_quantity", "quantity", "open_close", "account", "argument_list", "id", "status_msg", "batch_flag", "algo_flag"]
batch_table = pd.DataFrame(columns = batch_head)
algo_table = pd.DataFrame(columns = algo_head)
order_table = pd.DataFrame(columns = order_head)

tailLogExitFlag = True
regex = re.compile(r'<<<(.*?)>>>')

#a buffer to concat texts in single line while from two read trial, similar to the bluetooth project
readbuf = ""

class logHandler:
  @staticmethod
  def logTail():
    fp = open(logFile, 'r')
    md = mo.MonitorData();
    md.initTables(fp)
    print("init finish")
    init_table_finish_flag = 2
    while tailLogExitFlag:
      new = fp.readline()
      if new:
        global readbuf
        readbuf = readbuf + new
        li = readbuf.split('\n')
        if len(li)<2:
          continue
        for i in range(0,len(li)-1):
          match = regex.findall(li[i]);
          for item in match:
            print(item)
            #line_buf.append(item)
            SocketHandler.send_to_all({'type': 'sys', 'message': item,})
            mddata.logUpdate(item)
        readbuf = li[len(li)-1]

class IndexHandler(tornado.web.RequestHandler):
  def get(self):
    self.render("./index.html")

class SocketHandler(tornado.websocket.WebSocketHandler):
  clients = set()
  initFlag = False

 # def __init__(self, application, request, **kwargs):
 #  tornado.websocket.WebSocketHandler.__init__(self, application, request, **kwargs)

  @staticmethod
  def send_to_all(message):
    for c in SocketHandler.clients:
      c.write_message(message)

  @staticmethod
  def signalHandler(signum, frame):
    global tailLogExitFlag
    tailLogExitFlag = False
    exit(0)

  def on_message(self, message):
    SocketHandler.send_to_all({'type': 'user', 'id': id(self),
      'message':message,
    })

  def open(self):
    SocketHandler.clients.add(self)

    print ('new cli arrive')

    if (not SocketHandler.initFlag):
      Thread(target=logHandler.logTail).start()
      SocketHandler.initFlag = True

    #print(mddata.order_table.to_json())
    #self.write_message({'type': 'sys', 'table': mddata.order_table.to_json()})
    for i in range(0,len(mddata.order_table)):
      #print(mddata.order_table.iloc[i].to_json())
      self.write_message({'type': 'sys', 'table': mddata.order_table.iloc[i].to_json(),})
    for line in mddata.app_buf:
      self.write_message({'type': 'sys', 'message': line,})
      print(line)
    self.write_message({'type': 'sys', 'init_finish': {},})

  def on_close(self):
    SocketHandler.clients.remove(self)

    #SocketHandler.send_to_all(str(id(self)) + ' has left')

setting = {"static_path": os.path.join(os.path.dirname(__file__), "static")
        }

handlerList = [('/', IndexHandler),
               ('/soc', SocketHandler),
              ]

if __name__ == '__main__':
  mddata.initTables(fp)

  app = tornado.web.Application(handlerList, SocketHandler, **setting)
  app.listen(listenPort)

  signal.signal(signal.SIGINT, SocketHandler.signalHandler)
  tornado.ioloop.IOLoop.instance().start()
