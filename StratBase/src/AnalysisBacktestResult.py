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
      price = float(dayTran.loc[idx]["Price"])
      direction =dayTran.loc[idx]["Direction"]

      stratStat["tradeCount"] += 1
      if direction == "LONG_BUY":
        stratStat["longCount"] += 1
      else:
        stratStat["shortCount"] += 1

      if not (code in positions):
        positions[code] = {}
        if (direction == "LONG_BUY"):
          positions[code]["Qty"] = qty
        else:
          positions[code]["Qty"] = -qty
        positions[code]["LastPrice"] = price
        continue

      posQty = positions[code]["Qty"]
      posAvg = positions[code]["LastPrice"]
      tranRet = 0.0

      if (posQty > 0 and direction == "LONG_BUY"):
        positions[code]["LastPrice"] = ((posQty * posAvg + qty * price) /
          (qty + posQty))
        positions[code]["Qty"] += qty

      elif (posQty < 0 and direction == "LONG_BUY"):
        if (posQty + qty > 0):
          tranRet = ((posAvg - price) / posAvg) * abs(posQty)
          positions[code]["Qty"] += qty
          positions[code]["LastPrice"] = price
        elif (posQty + qty < 0):
          tranRet = ((posAvg - price) / posAvg) * qty
          positions[code]["Qty"] += qty
        else:
          tranRet = ((posAvg - price) / posAvg) * abs(posQty)
          positions.pop(code, None)

        stratStat["longRet"] += tranRet
        if (tranRet > 0):
          helper["longWinRet"] += tranRet
          helper["longWinCount"] += 1

      elif (posQty > 0 and direction == "SHORT_SELL"):
        if (posQty - qty > 0):
          tranRet = ((price - posAvg) / posAvg) * qty
          positions[code]["Qty"] -= qty

        elif (posQty - qty < 0):
          tranRet = ((price - posAvg) / posAvg) * posQty
          positions[code]["Qty"] -= qty
          positions[code]["LastPrice"] = price
        else:
          tranRet = ((price - posAvg) / posAvg) * qty
          positions.pop(code, None)

        stratStat["shortRet"] += tranRet
        if (tranRet > 0):
          helper["shortWinRet"] += tranRet
          helper["shortWinCount"] += 1

      elif (posQty < 0 and direction == "SHORT_SELL"):
        positions[code]["LastPrice"] = ((-posQty * posAvg + qty * price) /
          (qty - posQty))
        positions[code]["Qty"] -= qty

      print "date:", date, "ret:", tranRet
      dayRet += tranRet
      #raw_input()

      #if (positions[code]["Qty"] > 0):
      #  stratStat["longRet"] += tranRet
      #  if (tranRet > 0):
      #    helper["longWinRet"] += tranRet
      #    helper["longWinCount"] += 1
      #else:
      #  stratStat["shortRet"] += tranRet
      #  if (tranRet > 0):
      #    helper["shortWinRet"] += tranRet
      #    helper["shortWinCount"] += 1

      #positions[code]["LastPrice"] = price

      #if (direction == "LONG_BUY"):
      #  positions[code]["Qty"] += qty
      #else:
      #  positions[code]["Qty"] -= qty

      #if (positions[code]["Qty"] == 0):
      #  positions.pop(code, None)

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

  if stratStat["longRet"] != helper["longWinRet"]:
    stratStat["longWinLoseRatio"] = (helper["longWinRet"] /
      abs(stratStat["longRet"] - helper["longWinRet"]))
  else:
    print "WARNING:longRet == longWinRet"

  if stratStat["shortRet"] != helper["shortWinRet"]:
    stratStat["shortWinLoseRatio"] = (helper["shortWinRet"] /
        abs(stratStat["shortRet"] - helper["shortWinRet"]))
  else:
    print "WARNING:shortRet == shortWinRet"

  stratStat["shortWinProb"] = 1 - stratStat["longWinProb"]

  pp = pprint.PrettyPrinter()
  pp.pprint(stratStat)
