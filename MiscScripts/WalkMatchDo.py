import os
import re

root = "/home/yuguess/ProcessFile/15min"
pattern =r"(.*)\.(DCE|CZC|SHF)"

def doFunc(path, fileName):
  fullFile = path + "/" + fileName
  print "remove ", fullFile
  os.remove(fullFile)

#######################################

regex = re.compile(pattern)

for (path, dirs, files) in os.walk(root):
  for fileName in files:
    if regex.match(str(fileName)):
      doFunc(path, fileName)
