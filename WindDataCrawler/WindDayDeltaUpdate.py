import os
import datetime as d
import pandas as pd
from WindPy import *

#######################
file_repo = r"\\192.168.0.56\DataShare\ProcessFile"
preFetchDays = -30
#######################

def tradingDayOffset(offset, dateStr):
    offsetDate = w.tdaysoffset(offset, dateStr, "").Data[0][0]
    return d.datetime.strftime(offsetDate, "%Y-%m-%d")


def read_last_line(fileName):
    for line in open(fileName, "r"):
        pass
    return line


def fetch_last_trading_day(date_str):
    #all high vol contract
    contracts = w.wset("sectorconstituent", "date=" + date_str + ";sectorid=1000015510000000").Data[1]
    prefetch_day = tradingDayOffset(-30, date_str)
    last_day_map = {}
    for contract in contracts:
        file_name = file_repo + "\\Day\\" + contract
        if not os.path.isfile(file_name):
            last_day_map[contract] = prefetch_day
            continue
        last_date_str = read_last_line(file_name).split(',')[0]
        last_day_map[contract] = last_date_str
    return last_day_map


def main():
    w.start()
    #today_str = get_today_date_string()
    today_str = "2016-11-01"
    lastday_map = fetch_last_trading_day(today_str)

    for contract, last_day in lastday_map.iteritems():
        print "fetching ", contract, last_day, "---", today_str
        hist = w.wsd(contract, "open,high,low,close,volume", last_day, today_str, "")
        time_strs = [d.strftime("%Y-%m-%d") for d in hist.Times]

        table = [[t, o, h, l, c, v] for t, o, h, l, c, v in
                 zip(time_strs, hist.Data[0], hist.Data[1], hist.Data[2], hist.Data[3], hist.Data[4])]
        df = pd.DataFrame(table, columns=["date", "open", "high", "low", "close", "volume"])

        file_name = file_repo + "\\Day\\" + contract
        if not os.path.isfile(file_name):
            df.to_csv(file_name, index=None)
            continue

        fstream = open(file_name, "a+")
        for idx, row in df.iterrows():
            date_str = str(df.loc[idx]["date"])
            o = str(df.loc[idx]["open"])
            h = str(df.loc[idx]["high"])
            l = str(df.loc[idx]["low"])
            c = str(df.loc[idx]["close"])
            v = str(df.loc[idx]["volume"])

            result = ",".join([date_str, o, h, l, c, v]) + "\n"
            print "append result", result
            fstream.write(result)
        fstream.close()

if __name__ == "__main__":
    main()
