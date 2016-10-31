#!/usr/bin/python
import subprocess
import sys
import getopt
import os

def start():
  #logStash log collector
  subprocess.Popen(['/home/infra/logstash-2.3.3/bin/logstash', '-f',
    '/home/infra/hydra/ShareConfig/receive.conf'])

  #Monitor Server
  subprocess.Popen(['/home/infra/hydra/LogMonitor/StatusServer.py'])

  subprocess.Popen(['/home/infra/hydra/LogMonitor/MonitorServer.py'])

  cmdPath = '/home/infra/hydra/OrderDispatcher/'
  subprocess.Popen([cmdPath + 'OrderDispatcher.py', '-f',
    cmdPath + 'config/OrderDispatcher.json'])

  cmdPath = '/home/infra/hydra/SmartOrderService/'
  subprocess.Popen([cmdPath + 'build/SmartOrderService', '-f',
    cmdPath + 'config/SmartOrderService.json'])

  cmdPath = '/home/infra/hydra/LtsMarketUpdate/'
  subprocess.Popen([cmdPath + 'build/LtsMarketUpdate', '-f',
    cmdPath + 'config/LtsMarketUpdate.json'])

def stop():
  subprocess.call('/home/infra/hydra/ShareConfig/stopLogStash.sh')
  subprocess.call(['pkill', 'MonitorServer'])

  subprocess.call(['pkill', '-f', 'LtsMarketUpdate'])
  subprocess.call(['pkill', '-f', 'SmartOrderService'])
  subprocess.call(['pkill', '-f', 'OrderDispatcher'])
  subprocess.call(['pkill', '-f', 'StatusServer'])

def service(argv):
  try:
    opts, args = getopt.getopt(argv,"se",["start","stop"])
  except getopt.GetoptError:
    print "parameter not recognized!"
    sys.exit(2)

  for opt, arg in opts:
    if opt in ("-s", "--start"):
      start()
    elif opt in ("-e", "--stop"):
      stop()
    else:
      print "no valid option"

if __name__ == "__main__":
  if (len(sys.argv) < 2):
    print "parameter not recognized!"
    sys.exit(0)
  service(sys.argv[1:])
