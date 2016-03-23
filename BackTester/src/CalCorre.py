# -*- coding: utf-8 -*-
import math
import numpy as np
import tushare as ts
from pandas import Series, DataFrame
import pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt



# calculates the mean
def mean(x):
  sum = 0.0
  for i in x:
       sum += i
  return sum / len(x)

# calculates the sample standard deviation
def sampleStandardDeviation(x):
  sumv = 0.0
  for i in x:
       sumv += (i - mean(x))**2
  return math.sqrt(sumv/(len(x)-1))

# calculates the PCC using both the 2 functions above
def pearson(x,y):
  scorex = []
  scorey = []

  for i in x:
      scorex.append((i - mean(x))/sampleStandardDeviation(x))

  for j in y:
      scorey.append((j - mean(y))/sampleStandardDeviation(y))

  # multiplies both lists together into 1 list (hence zip) and sums the whole list
  return (sum([i*j for i,j in zip(scorex,scorey)]))/(len(x)-1)

class CalCorre:
  def __init__(self):
    self.stocks = ts.get_stock_basics()

  def CorreOfClosePrice(self, start, end, industry):
    bankStocks = self.stocks[self.stocks['industry'] == industry]
    bankIndex = bankStocks.index.values
    result = DataFrame(index=bankIndex, columns=bankIndex)
    priceList = {}
    for item in bankIndex:
      tmpStock = ts.get_hist_data(item, start, end)
      priceList[item] = tmpStock['close'].values

    for stock in priceList:
      for corStock in priceList:
        if np.isnan(result.ix[stock, corStock]):
          tmp = pearson(priceList[stock], priceList[corStock])
          print stock, corStock, tmp
          result.ix[stock, corStock] = tmp
          result.ix[corStock, stock] = tmp
    return result

if __name__=='__main__':
  cal = CalCorre()
  result = cal.CorreOfClosePrice('2016-01-14', '2016-03-14', '银行')
  print result['000001'].order(ascending=False).index

  # plt.title('Pearson Correlation Value Between Stocks from Bank Industry')
  # plt.pcolor(result)
  # plt.xticks(np.arange(0.5, len(result.columns)), result.columns, rotation=30)
  # plt.yticks(np.arange(0.5, len(result.columns)), result.columns)
  # plt.savefig('correlation')

