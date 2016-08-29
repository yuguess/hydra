import os.path
import time
import signal
import sys
import re
import pandas as pd
import ast

batch_head = ["batch_id", "trade_quantity", "quantity"]
algo_head = ["algo_id", "code", "buy/sell", "trade_quantity", "quantity"]
order_head = ["status", "code", "price", "trade_quantity", "quantity", "open_close", "account", "argument_list", "id", "status_msg", "batch_flag", "algo_flag", "buy_sell"]
error_code = {
  '0': "CONFIRM",
  '1': "SEND_ERR",
  '2': "CANCEL_ERR",
  '3': 'BAD_ORDER',
  '4': 'DEAL',
  '5': "CANCELED",
  '6': "PARTIA_CANED",
  '7': "PARTIA_DEAL",
};

class MonitorData:
  init_table_finish_flag = 0
  app_buf = []

  order_position = {}
  order_num_count = 0
  algo_position = {}
  algo_num_count = 0
  batch_position = {}
  batch_num_count = 0

  algo_id_map = {}
  batch_id_map = {}
  regex = re.compile(r'<<<(.*?)>>>')

  batch_table = pd.DataFrame(columns = batch_head)
  algo_table = pd.DataFrame(columns = algo_head)
  order_table = pd.DataFrame(columns = order_head)

  def __init__(self):
    print("Data Structure Creating!")

  def insertOrder(self,dic):
    try:
      # error_code is used to represent status of the order
      self.order_table.loc[self.order_num_count] = ["new_order", dic['code'], dic['limit_price'], 0, int(dic['trade_quantity']), dic['open_close'], dic['account'], dic['argument_list'], dic['id'], " ", dic['batch_id'], dic['alg_order_id'], dic['buy_sell']]
      self.algo_id_map[dic['id']] = dic['alg_order_id']
      self.batch_id_map[dic['id']] = dic['batch_id']
    except Exception as e:
      print("order json resolve failed:")
      print(e)
    return

  def insertAlgo(self,dic):
    try:
      # error_code is used to represent status of the order
      self.algo_table.loc[self.algo_num_count] = [dic['alg_order_id'], dic['code'], dic['buy_sell'], 0, int(dic['trade_quantity'])]
    except Exception as e:
      print("algo json resolve failed\n")
      print(e)
    return

  def insertBatch(self,dic):
    try:
      # error_code is used to represent status of the order
      self.batch_table.loc[self.batch_num_count] = [dic['batch_id'], 0, int(dic['trade_quantity'])]
    except Exception as e:
      print("batch json resolve failed\n")
      print(e)
    return

  def updateOrder(self,dic):
    if self.order_position.has_key(dic['ref_id']) == True:
      position = self.order_position[dic['ref_id']]
      self.order_table.loc[position, 'trade_quantity'] = int(dic['trade_quantity']) + self.order_table.loc[position]['trade_quantity']
      self.order_table.loc[position, 'status'] = error_code[str(dic['error_code'])]
    else:
      print("reveive response pointing to unvalid order")
      #print(dic)
    return

  def updateAlgo(self,dic):
    if dic['cedar_msg_type'] == "TYPE_ORDER_REQUEST":
      position = self.algo_position[dic['alg_order_id']]
      self.algo_table.loc[position, 'quantity'] += int(dic['trade_quantity'])
      if dic['alg_order_id']=="1471403191520779":
        print("add to quantity")
        #print(dic)
    else:
      try:
        position = self.algo_position[self.algo_id_map[dic['ref_id']]]
        self.algo_table.loc[position, 'trade_quantity'] += int(dic['trade_quantity'])
      except:
        return

  def updateBatch(self,dic):
    if dic['cedar_msg_type'] == "TYPE_ORDER_REQUEST":
      position = self.batch_position[dic['batch_id']]
      self.batch_table.loc[position, 'quantity'] += int(dic['trade_quantity'])
    else:
      try:
        position = self.batch_position[self.batch_id_map[dic['ref_id']]]
        self.batch_table.loc[position, 'trade_quantity'] += int(dic['trade_quantity'])
      except:
        return

  def onNewOrder(self,dic):
    if self.order_position.has_key(dic['id']) == False:
      self.order_position[dic['id']] = self.order_num_count
      self.insertOrder(dic)
      self.order_num_count += 1
    else:
      print("order request received twice, please check your log file\n")

    if self.algo_position.has_key(dic['alg_order_id']) == False:
      self.algo_position[dic['alg_order_id']] = self.algo_num_count
      self.insertAlgo(dic)
      self.algo_num_count += 1
    else:
      self.updateAlgo(dic)

    if self.batch_position.has_key(dic['batch_id']) == False:
      self.batch_position[dic['batch_id']] = self.batch_num_count
      self.insertBatch(dic)
      self.batch_num_count += 1
    else:
      self.updateBatch(dic)

    return

  def onUpdate(self,dic):
    self.updateOrder(dic)
    self.updateAlgo(dic)
    self.updateBatch(dic)

  def logUpdate(self,item):
    item = item.replace("\\","")
    print item
    dic = ast.literal_eval(item)
    if dic['type']=="TYPE_CANCEL_ORDER_REQUEST":
      return
    elif dic['type'] == "APP_STATUS_MSG":
      self.app_buf.append(item)
      #print(item)
    elif dic['type'] == "TYPE_SMART_ORDER_REQUEST":
      self.app_buf.append(item)
    elif dic['type'] == "TYPE_SMART_ORDER_REQUEST":
      return
    elif dic['cedar_msg_type'] == "TYPE_ORDER_REQUEST":
      self.onNewOrder(dic)
    elif dic['cedar_msg_type'] == "TYPE_RESPONSE_MSG":
      self.onUpdate(dic)
    return

  def initTables(self,fp):
    print("init")
    coo = 0
    oo = open('./17','w')
    while True:

      new = fp.readline()

      if new:
        match = self.regex.findall(new);
        for item in match:
          #print(item)
          self.logUpdate(item)
          #print(self.order_table)
          #print(self.algo_table)

      else:
        break
    #print(self.order_table)
    self.order_table.to_csv('./ss')
    self.algo_table.to_csv('./ssa')
    self.batch_table.to_csv('./ssb')
    #print(self.algo_table)
    print("finish")
    oo.close()
    return


