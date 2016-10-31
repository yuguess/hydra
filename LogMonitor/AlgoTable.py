import copy
import json

algo_head = ["alg_order_id", "code", "buy_sell", "trade_quantity", "quantity",
             "account", "ref_price", "trade_price", "slippage", "notional",
             "batch_id"]


class AlgoTable:
    algo_table = {}

    def __init__(self):
        return

    def process_message(self, data, orderTable):
        if data['type'] == "TYPE_SMART_ORDER_REQUEST" or data['type'] == "TYPE_SMALL_ORDER_REQUEST":
            result = self.on_smartorder(data, orderTable)
            return result
        elif data['type'] == "TYPE_FIRST_LEVEL_ORDER_REQUEST":
            result = self.on_firstlevel(data, orderTable)
            return result
        elif data['type'] == "APP_STATUS_MSG":
            result = self.on_appstatus(data, orderTable)
            return result
        elif data['cedar_msg_type'] == "TYPE_RESPONSE_MSG":
            result = self.on_update(data, orderTable)
            return result
        elif data['cedar_msg_type'] == "TYPE_ORDER_REQUEST":
            result = self.on_create(data, orderTable)
            return result

    def on_create(self, data, orderTable):
        order = data
        if not data["alg_order_id"] in self.algo_table:
            row = {"alg_order_id": order["alg_order_id"], 
                    "code": order["code"],
                    "buy_sell": order["buy_sell"], "trade_quantity": 0,
                    "quantity": "",
                    "account": order["account"], 
                    "ref_price": "",
                    "trade_price": "", 
                    "slippage": " ", 
                    "notional": "",
                    "batch_id": order["batch_id"]}
            self.algo_table[data["alg_order_id"]] = row
        else:
            row = self.algo_table[data["alg_order_id"]]
            row["code"] = order["code"]
            row["buy_sell"] = order["buy_sell"]
            row["account"] = order["account"]
            row["batch_id"] = order["batch_id"]
            self.algo_table[data["alg_order_id"]] = row
        return [data["alg_order_id"], json.dumps(row)]

    def on_update(self, data, order_table):
        if data["error_code"] != 4 and data["error_code"] != 7:
            return ""
        if data["ref_id"] in order_table:
            algo_id = order_table[data["ref_id"]]["alg_order_id"]
            row = self.algo_table[algo_id]
            if algo_id in self.algo_table:
                row["trade_quantity"] += int(data['trade_quantity'])
                if row["notional"] == "":
                    row["notional"] = {"buy": 0, "sell": 0}
                ord = order_table[data["ref_id"]]
                direction=0
                if data["trade_quantity"]!=0:
                    if ord["buy_sell"] == "LONG_BUY":
                        direction = 1
                        row["notional"]["buy"]+=float(data["trade_quantity"])*float(ord["price"])
                        row["trade_price"] = row["notional"]["buy"] / float(
                            row['trade_quantity'])

                    elif ord["buy_sell"] == "SHORT_SELL":
                        direction = -1
                        row["notional"]["sell"] += float(data["trade_quantity"]) * \
                                                   float(ord["price"])

                        row["trade_price"] = row["notional"]["sell"]/float(row['trade_quantity'])
                    if row["ref_price"]!="" and abs(direction)==1:
                        row["slippage"] = -((row["trade_price"]/float(row["ref_price"]))-1) * direction * 10000
            return [row["alg_order_id"], json.dumps(row)]
        else:
            # print data
            return ""

    def on_appstatus(self, data, orderTable):
        order = data
        if not data["alg_order_id"] in self.algo_table:
            row = {"alg_order_id": order["alg_order_id"], 
                    "code": "",
                   "buy_sell": "", 
                   "trade_quantity": 0,
                   "quantity": "", 
                   "account": "", 
                   "ref_price": "",
                   "trade_price": "", 
                   "slippage": " ", 
                   "notional": ""}
            self.algo_table[data["alg_order_id"]] = row
            return [data["alg_order_id"], json.dumps(
                self.algo_table[data["alg_order_id"]])]
        else:
            row = self.algo_table[data["alg_order_id"]]
            row["ref_price"] = data["values"][0]
            #return [row["alg_order_id"], json.dumps(row)]
	    return ""

    def on_smartorder(self, data, orderTable):
        order = data
        if not data["alg_order_id"] in self.algo_table:
            row = {"alg_order_id": order["alg_order_id"],
                    "code": order["code"],
                    "buy_sell": order["buy_sell"], 
                    "trade_quantity": 0,
                    "quantity": order["trade_quantity"], 
                    "account": "",
                    "ref_price": "",
                    "trade_price": "", 
                    "slippage": " ", 
                    "notional": "", 
                    "batch_id":data["batch_id"]}
            self.algo_table[data["alg_order_id"]] = row

            return [data["alg_order_id"], json.dumps(
                self.algo_table[data["alg_order_id"]])]
        else:
            row = self.algo_table[data["alg_order_id"]]
            row["quantity"] = order["trade_quantity"]
            row['batch_id']=data['batch_id']
	    #return [row["alg_order_id"], json.dumps(row)]
            return ""

    def on_firstlevel(self, data, orderTable):

        order = data
        if not data["alg_order_id"] in self.algo_table:
            row = {"alg_order_id": order["alg_order_id"], 
                    "code": order["code"],
                    "buy_sell": order["buy_sell"], 
                    "trade_quantity": 0,
                    "quantity": order["trade_quantity"], 
                    "account": "",
                    "ref_price": "",
                    "trade_price": "", 
                    "slippage": " ", 
                    "notional": "", 
                    "batch_id":data["batch_id"]}
            self.algo_table[data["alg_order_id"]] = row
            return [data["alg_order_id"], json.dumps(
                self.algo_table[data["alg_order_id"]])]
        else:
            row = self.algo_table[data["alg_order_id"]]
            row["quantity"] = order["trade_quantity"]
            row["batch_id"] = data["batch_id"]
            #return [row["alg_order_id"], json.dumps(row)]
	    return ""
