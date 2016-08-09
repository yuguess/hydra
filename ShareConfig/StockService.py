import subprocess
import sys
import getopt
import os

def start():
  subprocess.Popen(['/home/infra/logstash-2.3.3/bin/logstash', '-f', '/home/infra/hydra/ShareConfig/receive.conf'])
  subprocess.Popen(['/home/infra/hydra/LogMonitor/MonitorServer.py'])

def stop():
  subprocess.call('/home/infra/hydra/ShareConfig/stopLogStash.sh')
  subprocess.call(['pkill', 'MonitorServer'])

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
