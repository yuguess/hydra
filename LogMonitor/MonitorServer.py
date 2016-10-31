#!/usr/bin/python
import os.path
import signal
import re
import pandas as pd
import time
import datetime
import tornado.websocket
import MonitorData as mo
from threading import Thread
import json
import copy

today = datetime.date.today()
oneday = datetime.timedelta(days=1)
yesterday = today - oneday

########config ##############
logFile = "/home/ironman/MonitorLog/out"
#logFile = "C:/monitor_dev/test"
# logFile = "/home/infra/MonitorLog/" + yesterday.strftime("%Y-%m-%d")
#logFile = "C:/monitor_dev/2016-10-11"
#logFile = "C:/Users/hawk/Desktop/LogMonitor/2016-10-18"
#logFile = "/home/ironman/MonitorLog/" + time.strftime("%Y-%m-%d")
#$logFile = "/home/infra/MonitorLog/2016-10-18"
# logFile = "/home/mwan/LogMonitor/log"
# logFile_app = "/home/infra/MonitorLog/ALGO_"+time.strftime("%Y-%m-%d")
# logFile = "/home/mwan/LogMonitor/log1"
listenPort = "8082"
#############################
print(logFile)

init_table_finish_flag = 0
app_line_buf = []
mddata = mo.MonitorData()

try:
    fp = open(logFile, 'r')
except:
    print("open failed, pliz check")
    fp = open(logFile, 'w')
fp.close()
fp = open(logFile, 'r')

tailLogExitFlag = True
regex = re.compile(r'<<<(.*?)>>>')

# a buffer to concat texts in single line while from two read trial, similar to the bluetooth project
readbuf = ""

def process_result(result_in):
    result = copy.deepcopy(result_in)
    if result==None:
        return

    if result[0] != "" and result[0] != None:
        try:
            SocketHandler.send_to_all(
                {'type': 'batch', 'key': result[0][0], 'value': result[0][1]})
        except:
            print result
    if result[1] != "" and result[1] != None:
        try:
            SocketHandler.send_to_all(
                {'type': 'algo', 'key': result[1][0], 'value': result[1][1]})
        except:
            print result
    if result[2] != "" and result[2] != None:
        try:
            SocketHandler.send_to_all(
                {'type': 'order', 'key': result[2][0], 'value': result[2][1]})
        except:
            print result

class logHandler:
    @staticmethod
    def logTail():
        # fp = open(logFile, 'r')
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
                if len(li) < 2:
                    continue
                for i in range(0, len(li) - 1):
                    match = regex.findall(li[i])
                    if len(match) > 1:
                        match = match[1:2]
                    for item in match:
                        # print(item)
                        # line_buf.append(item)
                        result = mddata.logUpdate(item)
                        process_result(result)
                readbuf = li[len(li) - 1]

class IndexHandler(tornado.web.RequestHandler):
    def get(self):
        self.render("./index.html")

class SocketHandler(tornado.websocket.WebSocketHandler):
    clients = set()
    initFlag = False

    def check_origin(self, origin):
        return True

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
                                   'message': message,
                                   })

    def open(self):
        SocketHandler.clients.add(self)

        print('new cli arrive')

        if (not SocketHandler.initFlag):
            Thread(target=logHandler.logTail).start()
            SocketHandler.initFlag = True

        # print(mddata.order_table.to_json())
        # self.write_message({'type': 'sys', 'table': mddata.order_table.to_json()})
        for k, v in mo.ot.OrderTable.order_table.iteritems():
            self.write_message(
                {'type': 'order', 'key': str(k), 'value': str(v)})
        for k, v in mo.at.AlgoTable.algo_table.iteritems():
            self.write_message(
                {'type': 'algo', 'key': str(k), 'value': str(v)})
        for k, v in mo.bt.BatchTable.batch_table.iteritems():
            self.write_message(
                {'type': 'batch', 'key': str(k), 'value': str(v)})

    def on_close(self):
        SocketHandler.clients.remove(self)

        # SocketHandler.send_to_all(str(id(self)) + ' has left')

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
