#ifndef ENUM_STRING_MAP_H
#define ENUM_STRING_MAP_H

#include <iostream>
#include <map>
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "CedarLogger.h"

class StringToEnum {
public:
  static ExchangeType toExchangeType(std::string &exchange) {
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

  static StrategyMode toStrategyMode(std::string &strategyModeStr) {
    std::map<std::string, StrategyMode> stratToEnum = {
      {"Backtest", BACKTEST},
      {"Livetest", LIVETEST},
      {"LiveTrading", LIVE_TRADING},
    };

    if (stratToEnum.find(strategyModeStr) == stratToEnum.end())
      LOG(FATAL) << " Invalid strategyMode string " << strategyModeStr;
    return stratToEnum[strategyModeStr];
  }
};

class EnumToString {
public:
  static std::string toString(ExchangeType exchangeType) {
    std::map<ExchangeType, std::string> exchangeTypeToString = {
      {SHSE, "SHSE"},
      {SZSE, "SZSE"},
      {CFE, "CFE"},
      {SHFE, "SHFE"},
      {DCE, "DCE"},
      {ZCE, "ZCE"},
    };

    if (exchangeTypeToString.find(exchangeType) == exchangeTypeToString.end())
      LOG(FATAL) << " invalid cedar msg type " << exchangeType;
    return exchangeTypeToString[exchangeType];
  }

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

  static std::string toString(RequestType reqType) {
    static std::map<RequestType, std::string> reqMsgTypeToString = {
      {TYPE_LIMIT_ORDER_REQUEST, "TYPE_LIMIT_ORDER_REQUEST"},
      {TYPE_MARKET_ORDER_REQUEST, "TYPE_MARKET_ORDER_REQUEST"},
      {TYPE_CANCEL_ORDER_REQUEST, "TYPE_CANCEL_ORDER_REQUEST"},
      {TYPE_SMART_ORDER_REQUEST, "TYPE_SMART_ORDER_REQUEST"},
      {TYPE_SMALL_ORDER_REQUEST, "TYPE_SMALL_ORDER_REQUEST"},
      {TYPE_FIRST_LEVEL_ORDER_REQUEST, "TYPE_FIRST_LEVEL_ORDER_REQUEST"},
    };

    if (reqMsgTypeToString.find(reqType) == reqMsgTypeToString.end())
      LOG(FATAL) << " invalid request msg type " << reqType;
    return reqMsgTypeToString[reqType];
  }

  static std::string toString(TradeDirection tradeDir) {
    static std::map<TradeDirection, std::string> tradeDirToString = {
      {LONG_BUY, "LONG_BUY"},
      {SHORT_SELL, "SHORT_SELL"},
    };

    if (tradeDirToString.find(tradeDir) == tradeDirToString.end())
      LOG(FATAL) << " Invalid trade dir " << tradeDir;
    return tradeDirToString[tradeDir];
  }

  static std::string toString(PositionDirection posDir) {
    static std::map<PositionDirection, std::string> posDirToString = {
      {OPEN_POSITION, "OPEN"},
      {CLOSE_POSITION, "CLOSE"},
      {CLOSE_TODAY_POSITION, "CLOSE_TODAY"},
      {CLOSE_YESTERDAY_POSITION, "CLOSE_YESTERDAY"},
    };

    if (posDirToString.find(posDir) == posDirToString.end())
      LOG(FATAL) << " Invalid trade dir " << posDir;
    return posDirToString[posDir];
  }

  static std::string toString(AppStatusMsgType statusMsgType) {
    static std::map<AppStatusMsgType, std::string> statusToString = {
      {APP_STATUS_MSG, "APP_STATUS_MSG"},
    };

    if (statusToString.find(statusMsgType) == statusToString.end())
      LOG(FATAL) << " Invalid statusMsgType " << statusMsgType;
    return statusToString[statusMsgType];
  }

};
#endif
