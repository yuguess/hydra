import json
import re
import os
import datetime as d
import pandas as pd
import getopt
from WindPy import *

#######################
# frequency in minutes
freq = "15"
sessionMap = [["09:00:00", "10:15:00"], ["10:30:00", "11:30:00"], ["13:00:00", "15:00:00"]]
fileRepo = r"\\192.168.0.56\DataShare\ProcessFile"
preFetchDays = -30
#######################

def filterByTradingSessionMap(df, dateStr):
    frames = []
    for (sBeg, sEnd) in sessionMap:
        # print sBeg, sEnd
        sBeg = d.datetime.strptime(dateStr + " " + sBeg, "%Y%m%d %H:%M:%S")
        sEnd = d.datetime.strptime(dateStr + " " + sEnd, "%Y%m%d %H:%M:%S")
        sessionDF = df[(df["date"] > sBeg) & (df["date"] <= sEnd)]
        frames.append(sessionDF)

    return pd.concat(frames)


def get_today_date_string():
    return d.datetime.now().strftime("%Y-%m-%d")


def isTradingDay(dateStr):
    return (not len(w.tdays(dateStr, dateStr, "").Times) == 0)


def getWindCode(dateStr):
    return w.wset("sectorconstituent", "date=" + dateStr + ";sectorid=1000015510000000").Data[1]


def addOneDay(dateStr):
    plusOneDate = d.datetime.strptime(dateStr, "%Y-%m-%d") + d.timedelta(days=1)
    return d.datetime.strftime(plusOneDate, "%Y-%m-%d")


def fetchWindDataByCode(wcode, begDateStr, endDateStr):
    print "fetching data ", wcode, begDateStr, "--", endDateStr

    hist = w.wsi(wcode, "open,high,low,close,volume", begDateStr, endDateStr, "BarSize=" + freq)
    table = [[t, o, h, l, c, v] for t, o, h, l, c, v in
             zip(hist.Times, hist.Data[0], hist.Data[1], hist.Data[2], hist.Data[3], hist.Data[4])]
    df = pd.DataFrame(table, columns=["date", "open", "high", "low", "close", "volume"])

    df['date'] = df['date'].dt.strftime("%Y-%m-%d %H:%M:%S")
    df['date'] = pd.to_datetime(df["date"], format="%Y-%m-%d %H:%M:%S")
    df = df.set_index(df['date'])

    minDate = d.datetime.strptime(begDateStr, "%Y-%m-%d")
    maxDate = d.datetime.strptime(endDateStr, "%Y-%m-%d")

    datelist = [(minDate + d.timedelta(days=i)) for i in range((maxDate - minDate).days + 1)]

    for date in datelist:
        dateStr = d.datetime.strftime(date, "%Y%m%d")

        dayTran = df[(df["date"] >= date) & (df['date'] < (date + d.timedelta(days=1)))]

        if len(dayTran.index) == 0:
            continue

        dayTran = filterByTradingSessionMap(dayTran, dateStr)

        fileName = fileRepo + "\\" + freq + "min\\" + dateStr + "\\" + wcode

        if not os.path.exists(os.path.dirname(fileName)):
            os.makedirs(os.path.dirname(fileName))
        fstream = open(fileName, "w")

        for idx, row in dayTran.iterrows():
            # if (dayTran.loc[idx]["volume"] <= 2):
            # continue

            startStr = d.datetime.strftime(dayTran.loc[idx]["date"] - d.timedelta(seconds=int(freq) * 60),
                                           format="%H:%M:%S")
            endStr = d.datetime.strftime(dayTran.loc[idx]["date"], format="%H:%M:%S")

            o = str(dayTran.loc[idx]["open"])
            h = str(dayTran.loc[idx]["high"])
            l = str(dayTran.loc[idx]["low"])
            c = str(dayTran.loc[idx]["close"])
            v = str(dayTran.loc[idx]["volume"])

            result = ",".join([dateStr, startStr, endStr, o, h, l, c, v]) + "\n"
            fstream.write(result)

        if fstream != None:
            fstream.close()
        print fileName


def extractContractCode(contract):
    return re.split('(\d+)', contract)[0].lower()


def contractsFilterByConfig(contracts, tradingSession):
    return filter(lambda c: (extractContractCode(c) in tradingSession), contracts)


def fetchWindData(dateStr, contracts):
    for contract in contracts:
        fetchWindDataByCode(contract, dateStr, addOneDay(dateStr))


def tradingDayOffset(offset, dateStr):
    offsetDate = w.tdaysoffset(offset, dateStr, "").Data[0][0]
    return d.datetime.strftime(offsetDate, "%Y-%m-%d")


def getContractsCode(dateStr):
    tradingSession = json.load(open('./TradingSession.json'))
    contracts = getWindCode(dateStr)
    return contractsFilterByConfig(contracts, tradingSession)


def isChangeMainContract(contracts, dateStr):
    lastTradingDay = getContractsCode(tradingDayOffset(-1, dateStr))
    return filter(lambda c: (not (c in lastTradingDay)), contracts)


def fetch_wind_data_datelist(datelist):
    for dateStr in datelist:
        print dateStr
        contracts = getContractsCode(dateStr)
        preFetchList = isChangeMainContract(contracts, dateStr)
        if (preFetchList):
            print dateStr, " changing contract to ", preFetchList
            print "prefetch ", tradingDayOffset(preFetchDays, dateStr), dateStr
            for contract in preFetchList:
                fetchWindDataByCode(contract, tradingDayOffset(preFetchDays, dateStr), dateStr)
        fetchWindData(dateStr, contracts)


def print_help():
    print "python ./WindDateUpdate"
    print "\t -u to update for today"


def main():
    min_date = get_today_date_string()
    max_date = get_today_date_string()

    w.start()

    days = w.tdays(min_date, max_date, "")
    datelist = [d.datetime.strftime(ddate, "%Y-%m-%d") for ddate in days.Times]

    if len(datelist) == 0:
        sys.exit(0)

    fetch_wind_data_datelist(datelist)


if __name__ == "__main__":
    main()
