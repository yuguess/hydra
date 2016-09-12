import sys
import getopt
import json
import datetime
import pprint
import pandas as pd


def helpMsg():
  print "AnalysisBacktestResult -f /path/to/resultFile"

if __name__ == '__main__':
  btJson = ""

  if (len(sys.argv) < 2):
    helpMsg()
    sys.exit(0)

  try:
    opts, args = getopt.getopt(sys.argv[1:], "f:", ["config="])
  except getopt.GetoptError:
    helpMsg()
    sys.exit(2)

  for opt, arg in opts:
    if opt in ("-f", "--config"):
      btJson = arg
    else:
      helpMsg()
      sys.exit(2)

  res = json.load(file(btJson))

  df = pd.io.json.json_normalize(res, "Transactions")
  df['Timestamp'] = pd.to_datetime(df["Timestamp"], format="%Y-%m-%d %H:%M:%S")
  #df = df.set_index(df["Timestamp"])

  startDateStr = res["Range"]["Start"]
  endDateStr = res["Range"]["End"]

  start = datetime.datetime.strptime(startDateStr, "%Y%m%d")
  end = datetime.datetime.strptime(endDateStr, "%Y%m%d")
  #datelist = [(start + datetime.timedelta(days = i)).strftime('%Y%m%d') \
  #    for i in range((end - start).days + 1)]

  datelist = [(start + datetime.timedelta(days = i)) \
      for i in range((end - start).days + 1)]

  positions = {}
  accumRet = 1
  retList = []
  stratStat = {}
  stratStat["tradeCount"] = 0
  stratStat["longCount"] = 0
  stratStat["shortCount"] = 0
  stratStat["longRet"] = 0
  stratStat["shortRet"] = 0


  helper = {}
  helper["longWinRet"] = 0
  helper["longWinCount"] = 0
  helper["shortWinRet"] = 0
  helper["shortWinCount"] = 0

  for date in datelist:
    dayTran = df[(df["Timestamp"] >= date) & (df['Timestamp'] <
        (date + datetime.timedelta(days = 1)))]

    dayRet = 0

    for idx, row in dayTran.iterrows():

      code = dayTran.loc[idx]["Code"]
      qty = dayTran.loc[idx]["Qty"]
      price =dayTran.loc[idx]["Price"]
      direction =dayTran.loc[idx]["Direction"]

      stratStat["tradeCount"] += 1
      if direction == "Buy":
        stratStat["longCount"] += 1
      else:
        stratStat["shortCount"] += 1

      if not (code in positions):
        positions[code] = {}
        if (direction == "Buy"):
          positions[code]["Qty"] = qty
        else:
          positions[code]["Qty"] = -qty
        positions[code]["LastPrice"] = price
        continue

      tranRet = ((price - positions[code]["LastPrice"]) /
        positions[code]["LastPrice"]) * positions[code]["Qty"]

      print "date:", date, "ret:", tranRet
      dayRet += tranRet

      if (positions[code]["Qty"] > 0):
        stratStat["longRet"] += tranRet
        if (tranRet > 0):
          helper["longWinRet"] += tranRet
          helper["longWinCount"] += 1
      else:
        stratStat["shortRet"] += tranRet
        if (tranRet > 0):
          helper["shortWinRet"] += tranRet
          helper["shortWinCount"] += 1

      positions[code]["LastPrice"] = price

      if (direction == "Buy"):
        positions[code]["Qty"] += qty
      else:
        positions[code]["Qty"] -= qty

      if (positions[code]["Qty"] == 0):
        positions.pop(code, None)

    #for code in positions.iteritems():
    #  dayRet += (closePrice[code][date] - positions[code]["LastPrice"]) /
    #    positions[code]["LastPrice"]
    #  positions[code]["LastPrice"] = closePrice[code][date]

    accumRet += dayRet
    retList.append(accumRet)

  for date, ret in zip(datelist, retList):
    print date, "---", ret

  preMax = 0
  maxDropDown = 0
  for ret in retList:
    preMax = max(ret, preMax)
    maxDropDown = max(maxDropDown, preMax - ret)

  stratStat["accumRet"] = accumRet - 1
  stratStat["retDropDownRatio"] = stratStat["accumRet"] / maxDropDown
  stratStat["maxDropDown"] = maxDropDown
  stratStat["longWinProb"] = (float(helper["longWinCount"]) /
    stratStat["longCount"])

  stratStat["longWinLoseRatio"] = (helper["longWinRet"] /
    abs(stratStat["longRet"] - helper["longWinRet"]))

  stratStat["shortWinLoseRatio"] = (helper["shortWinRet"] /
    abs(stratStat["shortRet"] - helper["shortWinRet"]))

  stratStat["shortWinProb"] = 1 - stratStat["longWinProb"]

  pp = pprint.PrettyPrinter()
  pp.pprint(stratStat)
