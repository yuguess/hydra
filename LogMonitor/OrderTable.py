import copy
import json

"""order_head = ["status", "code", "price", "trade_quantity", "quantity",
              "open_close", "account", "argument_list", "id",
              "status_msg", "batch_id", "alg_order_id", "buy_sell"]"""


class OrderTable:
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

    order_table = {}

    def __init__(self):
        return

    def process_message(self, data):
        if data['type'] == "TYPE_CANCEL_ORDER_REQUEST":
            return ""
        elif data['type'] == "APP_STATUS_MSG":
            result = self.on_appstatus(data)
            return result
        elif data['type'] == "TYPE_SMART_ORDER_REQUEST" or data['type'] == "TYPE_SMALL_ORDER_REQUEST":
            result = self.on_smartorder(data)
            return result
        elif data['type'] == "TYPE_FIRST_LEVEL_ORDER_REQUEST":
            result = self.on_firstlevel(data)
            return result
        elif data['cedar_msg_type'] == "TYPE_ORDER_REQUEST":
            result = self.on_create(data)
            return result
        elif data['cedar_msg_type'] == "TYPE_RESPONSE_MSG":
            result = self.on_update(data)
            return result
        else:
            print("unrecognized data!")
            return ""

    def on_create(self, data):
        if not data["id"] in self.order_table:
            order = data
            row = {"status": "new order", "code": order["code"],
                   "price": order["limit_price"], "trade_quantity": 0,
                   "quantity": order["trade_quantity"],
                   "open_close": order["open_close"],
                   "account": order["account"],
                   "argument_list": order["argument_list"],
                   "id": order["id"], "batch_id": order["batch_id"],
                   "alg_order_id": order["alg_order_id"],
                   "buy_sell": order["buy_sell"], "trade_price":-1}
            self.order_table[data["id"]] = row
            return [data["id"], json.dumps(row)]
        else:
            # print data
            return ""

    def on_update(self, data):
        if data["ref_id"]=="1477551452622990":
            print data
        if not data["ref_id"] in self.order_table:
            return ""
        else:
            # print "hit"
            row = self.order_table[data["ref_id"]]
            row["status"] = self.error_code[str(data['error_code'])]
            if data["error_code"] == 4 or data["error_code"] == 7:
                row["trade_quantity"] += int(data['trade_quantity'])
            return [data["ref_id"], json.dumps(row)]

    def on_appstatus(self, data):
        return ""

    def on_smartorder(self, data):
        return ""

    def on_firstlevel(self, data):
        return ""
