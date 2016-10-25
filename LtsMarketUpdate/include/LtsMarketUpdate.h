#ifndef LTS_MARKETUPDATE_H
#define LTS_MARKETUPDATE_H

#include "ProtoBufMsgHub.h"
#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "SecurityFtdcMdApi.h"

class LtsMarketUpdate : public CSecurityFtdcMdSpi {

public:
  LtsMarketUpdate();
  virtual ~LtsMarketUpdate();
  void run();

private:
  void OnFrontConnected();
  void OnFrontDisconnected(int nReason);
  void OnRspUserLogin(CSecurityFtdcRspUserLoginField *,
    CSecurityFtdcRspInfoField *, int, bool);
  void OnRspSubMarketData(
    CSecurityFtdcSpecificInstrumentField*,
    CSecurityFtdcRspInfoField *, int, bool);
  void OnRtnDepthMarketData(CSecurityFtdcDepthMarketDataField*);

  int onMsg(MessageBase);
  bool onMDReq(DataRequest &mdReq);
  bool ltsMDToCedarMD(CSecurityFtdcDepthMarketDataField*, MarketUpdate&);
  bool printMDStruct(CSecurityFtdcDepthMarketDataField*);

  CSecurityFtdcMdApi *pMdFrontMdApi;
  ProtoBufMsgHub msgHub;
  std::map<std::string, std::string> registerTickers;
};

#endif
