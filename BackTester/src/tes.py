import talib
import numpy
import random
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

data = []

for i in range(1,100):
  data.append(random.uniform(10,20))
print data

real = numpy.array(data)


macd, macdsignal, macdhist = talib.MACD(real, fastperiod = 12, slowperiod = 26, signalperiod = 9)

print macd, macdsignal, macdhist
plt.plot(numpy.zeros(len(data)), 'k')
plt.plot(macd, 'b')
plt.plot(macdsignal, 'g')
plt.plot(macdhist, 'r')
plt.savefig('myfig')

