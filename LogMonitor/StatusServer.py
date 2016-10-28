#!/usr/bin/python
import os
import time
import json
pro_list = ['OrderDispatcher','MonitorServer.p', 'SmartOrderServi']
pro_buf = {}

import os.path
import time
import signal
import sys
import re
import ast

import tornado.web
import tornado.ioloop
import tornado.websocket
from threading import Thread

#######config ###############
#logFile = "/home/infra/MonitorLog/2016-08-22"
logFile = "./futurelog.txt"
#logFile_app = "/home/mwan/LogMonitor/log"
#logFile_app = "/home/infra/MonitorLog/ALGO_"+time.strftime("%Y-%m-%d")
#logFile = "/home/mwan/LogMonitor/log1"
listenPort = "8001"
#############################
print logFile

init_table_finish_flag = 0
log_buf = []



tailLogExitFlag = True


def checkResult(result):
    process = []
    for line in result:
        line = line.replace('\n','')
        buf = line.split(' ')
        if len(buf) == 2:
            process.append(buf[1])
    #print(process)
    return process

def checkStatus():
    result = os.popen("pgrep -au infra").readlines()
    process = checkResult(result)
    for pro in pro_list:
        if pro in process:
            pro_buf[pro]=True
        else:
            pro_buf[pro]=False
    return json.dumps(pro_buf)
    #time.sleep(10)

class logHandler:

  @staticmethod
  def startStatServe():
    while True:
        tmp = checkStatus()
        SocketHandler.send_to_all({'type': 'process_status', 'process_status': tmp,})
        time.sleep(1)

class SocketHandler(tornado.websocket.WebSocketHandler):
  clients = set()
  initFlag = False

 # def __init__(self, application, request, **kwargs):
 #  tornado.websocket.WebSocketHandler.__init__(self, application, request, **kwargs)

  def check_origin(self, origin):
    return True

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
      Thread(target=logHandler.startStatServe).start()
      SocketHandler.initFlag = True

  def on_close(self):
    SocketHandler.clients.remove(self)

    #SocketHandler.send_to_all(str(id(self)) + ' has left')

setting = {"static_path": os.path.join(os.path.dirname(__file__), "static")
        }

handlerList = [
               ('/soc', SocketHandler),
              ]

if __name__ == '__main__':

  app = tornado.web.Application(handlerList, SocketHandler, **setting)
  app.listen(listenPort)

  signal.signal(signal.SIGINT, SocketHandler.signalHandler)
  tornado.ioloop.IOLoop.instance().start()








