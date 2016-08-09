#!/home/yuguess/anaconda2/bin/python
import os.path
import time
import signal
import sys
import re

import tornado.web
import tornado.ioloop
import tornado.websocket
from threading import Thread

#######config ###############
logFile = "/home/infra/CedarLog/" + time.strftime("%Y-%m-%d")
listenPort = "8000"
#############################

lineBuf = []
tailLogExitFlag = True
regex = re.compile(r'<<<(.*?)>>>')

class logHandler:

  @staticmethod
  def logTail():
    fp = open(logFile, 'r')
    while tailLogExitFlag:
      new =  fp.readline()
      if new:
        match = regex.findall(new);
        for item in match:
          print item
          lineBuf.append(item)
          SocketHandler.send_to_all({'type': 'sys', 'message': item,})
      else:
        time.sleep(0.5)

class IndexHandler(tornado.web.RequestHandler):
  def get(self):
    self.render("./index.html")

class SocketHandler(tornado.websocket.WebSocketHandler):
  clients = set()
  initFlag = False

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
      time.sleep(1)
    else:
      for line in lineBuf:
        self.write_message({'type': 'sys', 'message': line,})

  def on_close(self):
    SocketHandler.clients.remove(self)
    #SocketHandler.send_to_all(str(id(self)) + ' has left')

setting = {"static_path": os.path.join(os.path.dirname(__file__), "static")
        }

handlerList = [('/', IndexHandler),
               ('/soc', SocketHandler),
              ]

if __name__ == '__main__':
  app = tornado.web.Application(handlerList, SocketHandler, **setting)
  app.listen(listenPort)

  signal.signal(signal.SIGINT, SocketHandler.signalHandler)
  tornado.ioloop.IOLoop.instance().start()
