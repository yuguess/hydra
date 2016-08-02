#ifndef LOG_HELPER_H
#define LOG_HELPER_H

#include <json/json.h>
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "EnumStringMap.h"

#define LOG_TAG_HEADER "<<<"
#define LOG_TAG_FOOTER ">>>"

class LogHelper {
public:
  static bool logObject(ResponseMessage &rsp) {
    Json::Value jMsg;
    jMsg["msg_type"] =  cedarMsgTypeToString[TYPE_RESPONSE_MSG];
    jMsg["id"] = rsp.id();
    jMsg["ref_id"] = rsp.ref_id();
    jMsg["type"] = rspMsgTypeToString[rsp.type()];
    jMsg["trade_quantity"] = rsp.trade_quantity();
    jMsg["error_code"] = rsp.error_code();
    jMsg["error_msg"] = rsp.error_msg();

    toLogFile(jMsg);
    return true;
  }

  static bool logObject(OrderRequest &req) {
    return true;
  }

  static inline void toLogFile(Json::Value &jMsg) {
    static Json::FastWriter fastWriter;
    LOG(INFO) << LOG_TAG_HEADER << fastWriter.write(jMsg) << LOG_TAG_FOOTER;
  }

};

#endif
