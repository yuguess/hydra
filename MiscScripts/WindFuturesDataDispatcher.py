import pandas as pd
import os
import datetime


#codeList = "RB.SHF,M.DCE,I.DCE,RM.CZC,FG.CZC,P.DCE,MA.CZC,BU.SHF,C.DCE"
codeList = "JD1701.DCE"
resultPath = "~/WinShare/";
#in minutes
freq = "15"
dataRepoPath = "/home/yuguess/ProcessFile/"

#########################

def dispatchCodeData(code):
  freqStr = freq + "min"
  freqInSeconds = int(freq) * 60

  df = pd.read_csv(resultPath + code);

  df['timestamp'] = pd.to_datetime(df["date"], format="%Y-%m-%d %H:%M:%S")
  df = df.set_index(df['timestamp'])
  minDateStr = datetime.datetime.strftime(min(df.index), "%Y-%m-%d")
  maxDateStr = datetime.datetime.strftime(max(df.index), "%Y-%m-%d")

  minDate = datetime.datetime.strptime(minDateStr, "%Y-%m-%d")
  maxDate = datetime.datetime.strptime(maxDateStr, "%Y-%m-%d")

  datelist = [(minDate + datetime.timedelta(days = i)) \
        for i in range((maxDate - minDate).days + 1)]

  for date in datelist:
    dateStr = datetime.datetime.strftime(date, "%Y%m%d")
    dayTran = df[(df["timestamp"] >= date) & (df['timestamp'] <
          (date + datetime.timedelta(days = 1)))]

    fstream = None

    if len(dayTran.index) != 0:
      fileName = dataRepoPath + freqStr + "/" + dateStr + "/" + code

      #if (not os.path.isdir(dataRepoPath + freqStr + "/" + dateStr)):
      #  continue

      if (not os.path.exists(os.path.dirname(fileName))):
        os.makedirs(os.path.dirname(fileName))
      fstream = open(fileName, "w")

    for idx, row in dayTran.iterrows():
      if (dayTran.loc[idx]["volume"] <= 2):
        continue

      startStr = datetime.datetime.strftime(dayTran.loc[idx]["timestamp"] -
        datetime.timedelta(seconds = freqInSeconds), format = "%H:%M:%S")
      endStr = datetime.datetime.strftime(dayTran.loc[idx]["timestamp"],
        format = "%H:%M:%S")

      o = str(dayTran.loc[idx]["open"])
      h = str(dayTran.loc[idx]["high"])
      l = str(dayTran.loc[idx]["low"])
      c = str(dayTran.loc[idx]["close"])
      v = str(dayTran.loc[idx]["volume"])

      result = ",".join([dateStr, startStr, endStr, o, h, l, c, v]) + "\n"
      fstream.write(result)

    if (fstream != None):
      fstream.close()
    print dateStr

for code in codeList.split(','):
  print code
  dispatchCodeData(code)
