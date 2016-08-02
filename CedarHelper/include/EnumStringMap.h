#ifndef ENUM_STRING_MAP_H
#define ENUM_STRING_MAP_H

std::map<std::string, ExchangeType> exchangeStringToEnum = {
  {"SHSE", SHSE},
  {"SZSE", SZSE},
  {"CFE", CFE},
  {"SHFE", SHFE},
  {"DCE", DCE},
  {"ZCE", ZCE},
};

std::map<CedarMsgType, std::string> cedarMsgTypeToString = {
  {TYPE_ORDER_REQUEST, "TYPE_ORDER_REQUEST"},
  {TYPE_RESPONSE_MSG, "TYPE_RESPONSE_MSG"},
};

std::map<ResponseMsgType, std::string> rspMsgTypeToString = {
  {TYPE_NEW_ORDER_CONFIRM, "TYPE_ORDER_REQUEST"},
  {TYPE_CANCEL_ORDER_CONFIRM, "TYPE_CANCEL_ORDER_CONFIRM"},
  {TYPE_ERROR, "TYPE_ERROR"},
  {TYPE_TRADE, "TYPE_TRADE"},
};
#endif

