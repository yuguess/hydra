import re
import pandas as pd
import ast
import OrderTable as ot
import AlgoTable as at
import BatchTable as bt

pd.options.mode.chained_assignment = None

batch_head = ["batch_id", "trade_quantity", "quantity"]
algo_head = ["algo_id", "code", "buy/sell", "trade_quantity", "quantity"]
order_head = ["status", "code", "price", "trade_quantity", "quantity",
              "open_close", "account", "argument_list", "id",
              "status_msg", "batch_id", "algo_id", "buy_sell"]
error_code = {
    '0': "CONFIRM",
    '1': "SEND_ERR",
    '2': "CANCEL_ERR",
    '3': 'BAD_ORDER',
    '4': 'DEAL',
    '5': "CANCELED",
    '6': "PARTIA_CANED",
    '7': "PARTIA_DEAL",
}


class MonitorData:
    regex = re.compile(r'<<<(.*?)>>>')

    batch_table = pd.DataFrame(columns=batch_head)
    algo_table = pd.DataFrame(columns=algo_head)
    order_table = pd.DataFrame(columns=order_head)
    ord_table = ot.OrderTable()
    alg_table = at.AlgoTable()
    ba_table = bt.BatchTable()

    def __init__(self):
        print("Data Structure Creating!")

    def onUpdate(self, dic):
        self.updateOrder(dic)
        self.updateAlgo(dic)
        self.updateBatch(dic)

    def logUpdate(self, item):
        item = item.replace("\\", "")
        # print item
        try:
            dic = ast.literal_eval(item)
        except:
            return ["","",""]
        if type(dic) == list:
            #print dic
            return
        #if dic["cedar_msg_type"] == "TYPE_ERROR_MSG":
            #print dic
        #    return ["","","",dic]
        result_ord = self.ord_table.process_message(dic)
        result_batch = self.ba_table.process_message(dic,
                                                     self.ord_table.order_table,
                                                     self.alg_table.algo_table)
        result_algo = self.alg_table.process_message(dic,
                                                     self.ord_table.order_table)
        return [result_batch, result_algo, result_ord]

    def initTables(self, fp):
        # print("init")
        coo = 0
        while True:
            new = fp.readline()
            if new:
                match = self.regex.findall(new);
                if len(match) > 1:
                    match = match[1:2]
                for item in match:
                    self.logUpdate(item)
            else:
                break

        print("finish")
        return
