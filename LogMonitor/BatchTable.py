import json
import copy

batch_head = ["batch_id", "trade_quantity", "quantity", "notional", "account"]


class BatchTable:
    batch_table = {}
    algo_record = {}

    def __init__(self):
        return

    def process_message(self, data, orderTable, alg_table):
        if data['type'] == "TYPE_SMART_ORDER_REQUEST" or data['type'] == "TYPE_SMALL_ORDER_REQUEST":
            result = self.on_smartorder(data, orderTable, alg_table)
            return result
        elif data['type'] == "TYPE_FIRST_LEVEL_ORDER_REQUEST":
            result = self.on_firstlevel(data, orderTable, alg_table)
            return result
        elif data['type'] == "APP_STATUS_MSG":
            result = self.on_appstatus(data, orderTable)
            return result
        elif data['cedar_msg_type'] == "TYPE_RESPONSE_MSG":
            result = self.on_update(data, orderTable)
            return result
        elif data['cedar_msg_type'] == "TYPE_ORDER_REQUEST" and data['type'] != "TYPE_CANCEL_ORDER_REQUEST":
            result = self.on_create(data, orderTable)
            return result

    def on_create(self, data, order_table):
        # print data
        order = data
        if not data["batch_id"] in self.batch_table:
            row = {"batch_id": order["batch_id"], "trade_quantity": "",
                   "quantity": "", "notional": "", "account": data["account"]}
            self.batch_table[order["batch_id"]] = row
            return [row["batch_id"], json.dumps(row)]
        else:
            batch_id = order_table[data["id"]]["batch_id"]
            row = self.batch_table[batch_id]
            row["account"] = data["account"]
            return [row["batch_id"], json.dumps(row)]

    def on_update(self, data, order_table):
        if data["error_code"] != 4 and data["error_code"] != 7:
            return;
        if data["ref_id"] in order_table:
            batch_id = order_table[data["ref_id"]]["batch_id"]
            row = self.batch_table[batch_id]
            if batch_id in self.batch_table:
                if row["trade_quantity"] == "":
                    row["trade_quantity"] = 0
                row["trade_quantity"] += int(data['trade_quantity'])
                if row["notional"] == "":
                    row["notional"] = {"buy": 0, "sell": 0}
                ord = order_table[data["ref_id"]]
                direction = 0
                if data["trade_quantity"] != 0:
                    if ord["buy_sell"] == "LONG_BUY":
                        direction = 1
                        row["notional"]["buy"] += float(
                            data["trade_quantity"]) * float(ord["price"])
                        row["trade_price"] = row["notional"]["buy"] / float(
                            row['trade_quantity'])
                    elif ord["buy_sell"] == "SHORT_SELL":
                        direction = -1
                        row["notional"]["sell"] += float(
                            data["trade_quantity"]) * \
                                                   float(ord["price"])
                        row["trade_price"] = row["notional"]["sell"] / float(
                            row['trade_quantity'])

                return [row["batch_id"], json.dumps(row)]

        else:
            # print data
            return ""

    def on_appstatus(self, data, order_table):
        return ""

    def on_smartorder(self, data, order_table, alg_table):
        if data["alg_order_id"] in self.algo_record:
            return ""
        self.algo_record[data["alg_order_id"]] = 1
        order = data
        if not data["batch_id"] in self.batch_table:
            row = {"batch_id": order["batch_id"], "trade_quantity": 0,
                   "quantity": data["trade_quantity"], "notional": "", "account": data["account"]}
            self.batch_table[data["batch_id"]] = row
            return [data["batch_id"], json.dumps(row)]
        else:
            row = self.batch_table[data["batch_id"]]
            if row["quantity"] == "":
                row["quantity"] = 0
            row["quantity"] += int(data["trade_quantity"])
            row["acccount"]=data["account"]
	    return [row["batch_id"], json.dumps(row)]

    def on_firstlevel(self, data, orderTable, alg_table):
        if data["alg_order_id"] in alg_table and alg_table[data["alg_order_id"]]["quantity"]!="":
            return ""
        order = data
        if not data["batch_id"] in self.batch_table:
            row = {"batch_id": order["batch_id"], "trade_quantity": 0,
                   "quantity": data["trade_quantity"], "notional": "", "account": data["account"]}
            self.batch_table[data["batch_id"]] = row
            return [row["batch_id"], json.dumps(row)]
        else:
            row = self.batch_table[data["batch_id"]]
            if row["quantity"] == "":
                row["quantity"] = 0
            row["quantity"] += int(data["trade_quantity"])
            # if data["batch_id"]=="20161010_125431":
            #   print "" + str(row["quantity"])+","+ str(data)
            row["account"]=data["account"]
            return [row["batch_id"], json.dumps(row)]
