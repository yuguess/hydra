#ifndef ENUM_STRING_MAP_H
#define ENUM_STRING_MAP_H

#include <iostream>
#include <map>
#include "CPlusPlusCode/ProtoBufMsg.pb.h"

class StringToEnum {
public:
  static ExchangeType stringToExchangeType(std::string &exchange) {
    std::map<std::string, ExchangeType> exchangeStringToEnum = {
      {"SHSE", SHSE},
      {"SZSE", SZSE},
      {"CFE", CFE},
      {"SHFE", SHFE},
      {"DCE", DCE},
      {"ZCE", ZCE},
    };

    if (exchangeStringToEnum.find(exchange) == exchangeStringToEnum.end())
      LOG(FATAL) << " Invalid exchange string " << exchange;
    return exchangeStringToEnum[exchange];
  }
};

class EnumToString {
public:
  static std::string toString(CedarMsgType cedarMsgType) {
    std::map<CedarMsgType, std::string> cedarMsgTypeToString = {
      {TYPE_ORDER_REQUEST, "TYPE_ORDER_REQUEST"},
      {TYPE_RESPONSE_MSG, "TYPE_RESPONSE_MSG"},
    };

    if (cedarMsgTypeToString.find(cedarMsgType) == cedarMsgTypeToString.end())
      LOG(FATAL) << " invalid cedar msg type " << cedarMsgType;
    return cedarMsgTypeToString[cedarMsgType];
  }

  static std::string toString(ResponseMsgType rspMsgType) {
    static std::map<ResponseMsgType, std::string> rspMsgTypeToString = {
      {TYPE_NEW_ORDER_CONFIRM, "TYPE_ORDER_REQUEST"},
      {TYPE_CANCEL_ORDER_CONFIRM, "TYPE_CANCEL_ORDER_CONFIRM"},
      {TYPE_ERROR, "TYPE_ERROR"},
      {TYPE_TRADE, "TYPE_TRADE"},
    };

    if (rspMsgTypeToString.find(rspMsgType) == rspMsgTypeToString.end())
      LOG(FATAL) << " invalid response msg type " << rspMsgType;
    return rspMsgTypeToString[rspMsgType];
  }
};
#endif

