#ifndef GTAHELPER_H
#define GTAHELPER_H

#include "CedarTimeHelper.h"
#include "CedarHelper.h"
#include "DataAdapter.h"
#include "ProtoBufMsgHub.h"
#include "stdlib.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "json/json.h"

class GTAHelper {
  public:
  static std::string getDBName(std::string dataType, std::string code, boost::posix_time::ptime curTime) {
    std::string dbName = "GTA_";
    if (code.length() < 6) {
      LOG(FATAL) << "Stock or Future code not recognized!";
      return "000000";
    }
    if (code[0] == '6') {
      dbName += "SEL2_";
    } else if (code[0] == '0' || code[0] == '3') {
      if (dataType!="IndexData") {
        dbName += "SZL2_";
      } else {
        dbName += "SEL2_";
      }
    } else if (code[0] == 'I') {
      dbName += "FFL2_";
    } else if (code[0] = 'T') {
      dbName += "TFL2_";
    }

    if (dataType == "TaqData") {
      dbName += "TAQ_";
    } else if (dataType == "IndexData") {
      dbName += "INDEX_";
    } else if (dataType == "TrdminData") {
      dbName += "TRDMIN_";
    } else if (dataType == "OrderqueueData") {
      dbName += "ORDERQUEUE_";
    } else if (dataType == "TransactionData") {
      dbName += "TRANSACTION_";
    } else if (dataType == "VirtualauctionData") {
      dbName += "VIRTUALAUCTION_";
    } else if (dataType == "BulletinData") {
      dbName += "BULLETIN_";
    } else if (dataType == "OrderData") {
      dbName += "ORDER_";
    } else if (dataType == "StockinfoData") {
      dbName += "STOCKINFO_";
    } else if (dataType == "StockstatusData") {
      dbName += "STOCKSTATUS_";
    } else if (dataType == "TradeData") {
      dbName += "TRADE_";
    }

    dbName += CedarTimeHelper::ptimeToStr("%Y%m", curTime);


    return dbName;
  }

  static std::string getTableName(std::string dataType, std::string code, boost::posix_time::ptime curTime) {
    std::string tableName = "dbo.";

    if (code.length() < 6) {
      LOG(FATAL) << "Stock or Future code not recognized!";
      return "000000";
    }
    if (code[0] == '6') {
      tableName += "SHL2_";           //a stupid difference here with the DB name! sHl2 rather sEl2 here!
    } else if (code[0] == '0' || code[0] == '3') {
      if (dataType!="IndexData") {
        tableName += "SZL2_";
      } else {
        tableName += "SHL2_";
      }
    } else if (code[0] == 'I') {
      tableName += "FFL2_";
    } else if (code[0] = 'T') {
      tableName += "TFL2_";
    }

    if (dataType == "TaqData") {
      tableName += "TAQ_";

    } else if (dataType == "IndexData") {
      tableName += "INDEX_";
    } else if (dataType == "TrdminData") {
      tableName += "TRDMIN_";
    } else if (dataType == "OrderqueueData") {
      tableName += "ORDERQUEUE_";
    } else if (dataType == "TransactionData") {
      tableName += "TRANSACTION_";
    } else if (dataType == "VirtualauctionData") {
      tableName += "VIRTUALAUCTION_";
    } else if (dataType == "BulletinData") {
      tableName += "BULLETIN_";
    } else if (dataType == "OrderData") {
      tableName += "ORDER_";
    } else if (dataType == "StockinfoData") {
      tableName += "STOCKINFO_";
    } else if (dataType == "StockstatusData") {
      tableName += "STOCKSTATUS_";
    } else if (dataType == "TradeData") {
      tableName += "TRADE_";
    }

    tableName += code;
    tableName += '_';
    tableName += CedarTimeHelper::ptimeToStr("%Y%m", curTime);

    return tableName;
  }

  static std::string getSelCmd(std::string dataType, std::string code, boost::posix_time::ptime curTime) {
    std::string selCmd;
    if (dataType == "TaqData") {
      selCmd = selCmd_TAQ();
    } else if (dataType == "IndexData") {
      selCmd = selCmd_INDEX();
    } else if (dataType == "TrdminData") {
      selCmd = selCmd_TRDMIN();
    } else if (dataType == "OrderqueueData") {
      selCmd = selCmd_ORDERQUEUE();
    } else if (dataType == "TransactionData") {
      selCmd = selCmd_TRANSACTION();
    } else if (dataType == "VirtualauctionData") {
      selCmd = selCmd_VIRTUALAUCTION();
    } else if (dataType == "BulletinData") {
      selCmd = selCmd_BULLETIN();
    } else if (dataType == "OrderData") {
      selCmd = selCmd_ORDERDATA();
    } else if (dataType == "StockinfoData") {
      selCmd = selCmd_STOCKINFO();
    } else if (dataType == "StockstatusData") {
      selCmd = selCmd_STOCKSTATUS();
    } else if (dataType == "TradeData") {
      selCmd = selCmd_TRADE();
    }
    return selCmd + getTableName(dataType, code, curTime);
  }

  private:
  static std::string selCmd_TAQ() {
    return "select  [MSGSEQID]\
                              ,[SECURITYID]\
                              ,[TRDDATE]\
                              ,[DATATIMESTAMP]\
                              ,[PRECLOSEPX]\
                              ,[OPENPX]\
                              ,[HIGHPX]\
                              ,[LOWPX]\
                              ,[LASTPX]\
                              ,[INSTRUSTATUS]\
                              ,[NOOFFERLVL]\
                              ,[S10]\
                              ,[S09]\
                              ,[S08]\
                              ,[S07]\
                              ,[S06]\
                              ,[S05]\
                              ,[S04]\
                              ,[S03]\
                              ,[S02]\
                              ,[S01]\
                              ,[B01]\
                              ,[B02]\
                              ,[B03]\
                              ,[B04]\
                              ,[B05]\
                              ,[B06]\
                              ,[B07]\
                              ,[B08]\
                              ,[B09]\
                              ,[B10]\
                              ,[NOBIDLVL]\
                              ,[SV10]\
                              ,[SV09]\
                              ,[SV08]\
                              ,[SV07]\
                              ,[SV06]\
                              ,[SV05]\
                              ,[SV04]\
                              ,[SV03]\
                              ,[SV02]\
                              ,[SV01]\
                              ,[BV01]\
                              ,[BV02]\
                              ,[BV03]\
                              ,[BV04]\
                              ,[BV05]\
                              ,[BV06]\
                              ,[BV07]\
                              ,[BV08]\
                              ,[BV09]\
                              ,[BV10]\
                              ,[NUMTRADES]\
                              ,[TOTALVOLUME]\
                              ,[TURNOVER]\
                              ,[TOTALBIDQTY]\
                              ,[WAVGBIDPX]\
                              ,[ALTWAVGBIDPX]\
                              ,[TOTALOFFERQTY]\
                              ,[WAVGOFFERPX]\
                              ,[ALTWAVGOFFERPX]\
                              ,[IOPV]\
                              ,[YTM]\
                              ,[WARNTEXECQTY]\
                              ,[WARLOWERPX]\
                              ,[WARUPPERPX]\
                              ,[TIMESTAMP]\
                              ,[UNIX]\
                              ,[MARKET]   from  ";
  }

  static std::string selCmd_INDEX() {
    return "select   [MSGSEQID]\
                    ,[SECURITYID]\
                    ,[TRDDATE]\
                    ,[DATATIMESTAMP]\
                    ,[CLOSEINDEX]\
                    ,[OPENINDEX]\
                    ,[HIGHINDEX]\
                    ,[LOWINDEX]\
                    ,[LASTINDEX]\
                    ,[TOTALVOLUME]\
                    ,[TURNOVER]\
                    ,[TIMESTAMP]\
                    ,[UNIX]\
                    ,[MARKET] from ";
  }

  static std::string selCmd_TRDMIN() {
    return "SELECT   [IFCD]\
                    ,[TDATE]\
                    ,[TTIME]\
                    ,[OPNPRC]\
                    ,[CLSPRC]\
                    ,[HIPRC]\
                    ,[LOPRC]\
                    ,[CHG]\
                    ,[CHGPCT]\
                    ,[OPNINTRST]\
                    ,[MINTQ]\
                    ,[MINTM]\
                    ,[IFLXID]\
                    ,[IFLXNAME]\
                    ,[UNIX]\
                    ,[MARKET]\
                     FROM ";
  }

  static std::string selCmd_ORDERQUEUE() {
    return "SELECT   [MSGSEQID]\
                    ,[SECURITYID]\
                    ,[TRDDATE]\
                    ,[DATATIMESTAMP]\
                    ,[SIDE]\
                    ,[IMAGESTATUS]\
                    ,[NOPRCLEVEL]\
                    ,[PRCLVLOPERATOR]\
                    ,[PRICE]\
                    ,[NUMORDERS]\
                    ,[NOORDERS]\
                    ,[ORDERQTY01]\
                    ,[ORDERQTY02]\
                    ,[ORDERQTY03]\
                    ,[ORDERQTY04]\
                    ,[ORDERQTY05]\
                    ,[ORDERQTY06]\
                    ,[ORDERQTY07]\
                    ,[ORDERQTY08]\
                    ,[ORDERQTY09]\
                    ,[ORDERQTY10]\
                    ,[ORDERQTY11]\
                    ,[ORDERQTY12]\
                    ,[ORDERQTY13]\
                    ,[ORDERQTY14]\
                    ,[ORDERQTY15]\
                    ,[ORDERQTY16]\
                    ,[ORDERQTY17]\
                    ,[ORDERQTY18]\
                    ,[ORDERQTY19]\
                    ,[ORDERQTY20]\
                    ,[ORDERQTY21]\
                    ,[ORDERQTY22]\
                    ,[ORDERQTY23]\
                    ,[ORDERQTY24]\
                    ,[ORDERQTY25]\
                    ,[ORDERQTY26]\
                    ,[ORDERQTY27]\
                    ,[ORDERQTY28]\
                    ,[ORDERQTY29]\
                    ,[ORDERQTY30]\
                    ,[ORDERQTY31]\
                    ,[ORDERQTY32]\
                    ,[ORDERQTY33]\
                    ,[ORDERQTY34]\
                    ,[ORDERQTY35]\
                    ,[ORDERQTY36]\
                    ,[ORDERQTY37]\
                    ,[ORDERQTY38]\
                    ,[ORDERQTY39]\
                    ,[ORDERQTY40]\
                    ,[ORDERQTY41]\
                    ,[ORDERQTY42]\
                    ,[ORDERQTY43]\
                    ,[ORDERQTY44]\
                    ,[ORDERQTY45]\
                    ,[ORDERQTY46]\
                    ,[ORDERQTY47]\
                    ,[ORDERQTY48]\
                    ,[ORDERQTY49]\
                    ,[ORDERQTY50]\
                    ,[TIMESTAMP]\
                    ,[UNIX]\
                    ,[MARKET]\
                    FROM ";
  }

  static std::string selCmd_TRANSACTION() {
    return "SELECT     [MSGSEQID]\
                      ,[SECURITYID]\
                      ,[TRDDATE]\
                      ,[TRADETIME]\
                      ,[TRADEINDEX]\
                      ,[TRADECHANNEL]\
                      ,[TRADEPRICE]\
                      ,[TRADEQTY]\
                      ,[TURNOVER]\
                      ,[TIMESTAMP]\
                      ,[TRADEBUYNO]\
                      ,[TRADESELLNO]\
                      ,[TRADEBSFLAG]\
                      ,[UNIX]\
                      ,[MARKET]\
                      FROM ";
  }

  static std::string selCmd_VIRTUALAUCTION() {
    return "SELECT     [MSGSEQID]\
                      ,[SECURITYID]\
                      ,[TRDDATE]\
                      ,[DATATIMESTAMP]\
                      ,[PRICE]\
                      ,[VTLAUCTQTY]\
                      ,[LEAVEQTY]\
                      ,[SIDE]\
                      ,[TIMESTAMP]\
                      ,[UNIX]\
                      ,[MARKET]\
                      FROM ";
  }

  static std::string selCmd_BULLETIN() {
    return "SELECT [BULLETINID]\
                    ,[TRDDATE]\
                    ,[BULLETINTIME]\
                    ,[BULLETINNAME]\
                    ,[RAWDATALENGTH]\
                    ,[RAWDATA]\
                    ,[RECDATETIME]\
                    ,[UNIX]\
                    ,[MARKET]\
                    FROM ";
  }

  static std::string selCmd_ORDERDATA() {
    return "SELECT [SECURITYID]\
                    ,[TRDDATE]\
                    ,[ORDERENTRYTIME]\
                    ,[SETNO]\
                    ,[RECNO]\
                    ,[PRICE]\
                    ,[ORDERQTY]\
                    ,[ORDERKIND]\
                    ,[FUNCTIONCODE]\
                    ,[RECDATETIME]\
                    ,[UNIX]\
                    ,[MARKET]\
                    FROM ";
  }

  static std::string selCmd_STOCKINFO() {
    return "SELECT   [SECURITYID]\
                    ,[TRDDATE]\
                    ,[RECNO]\
                    ,[SYMBOL]\
                    ,[SECURITYDESC]\
                    ,[UNDERLYINGSECURITYID]\
                    ,[SECURITYIDSOURCE]\
                    ,[NOSECURITYALTID]\
                    ,[SECURITYALTID]\
                    ,[SECURITYALTIDSOURCE]\
                    ,[INDUSTRYCLASSIFICATION]\
                    ,[CURRENCY]\
                    ,[SZAREFACEVALUE]\
                    ,[OUTSTANDINGSZARES]\
                    ,[PUBLICFLOATSZAREQUANTITY]\
                    ,[PREVIOUSYEARPROFITPERSZARE]\
                    ,[CURRENTYEARPROFITPERSZARE]\
                    ,[NAV]\
                    ,[NOMISCFEES]\
                    ,[MISCFEETYPE]\
                    ,[MISCFEEBASIS]\
                    ,[MISCFEEAMT]\
                    ,[ISSUEDATE]\
                    ,[BONDTYPE]\
                    ,[COUPONRATE]\
                    ,[CONVERSIONPRICE]\
                    ,[AMERICANEUROPEAN]\
                    ,[CALLORPUT]\
                    ,[WARRANTCLEARINGTYPE]\
                    ,[CVRATIO]\
                    ,[CONVERSIONBEGINDATE]\
                    ,[CONVERSIONENDDATE]\
                    ,[INTERESTACCRUALDATE]\
                    ,[MATURITYDATE]\
                    ,[ROUNDLOT]\
                    ,[BIDLOTSIZE]\
                    ,[ASKLOTSIZE]\
                    ,[MAXFLOOR]\
                    ,[TRADEMETHOD]\
                    ,[PRICETICKSIZE]\
                    ,[PRICELIMITTYPE]\
                    ,[AUCTIONPRICELIMIT]\
                    ,[CONTINUOUSTRADEPRICELIMIT]\
                    ,[DAILYPRICEUPLIMIT]\
                    ,[DAILYPRICEDOWNLIMIT]\
                    ,[DAILYPRICEUPLIMIT2]\
                    ,[DAILYPRICEDOWNLIMIT2]\
                    ,[CONTRACTMULTIPLIER]\
                    ,[GAGERATE]\
                    ,[CRDBUYUNDERLYING]\
                    ,[CRDSELLUNDERLYING]\
                    ,[NOINDICESPARTICIPATED]\
                    ,[PARTICIPATINGINDEXID]\
                    ,[MARKETMAKERFLAG]\
                    ,[SECURITYEXCHANGE]\
                    ,[CFICODE]\
                    ,[SECURITYSUBTYPE]\
                    ,[SECURITYPROPERTIES]\
                    ,[SECURITYTRADINGSTATUS]\
                    ,[CORPORATEACTION]\
                    ,[TRADINGMECHANISM]\
                    ,[CRDPRICECHECKTYPE]\
                    ,[NETVOTINGFLAG]\
                    ,[SZRSTRUREFMFLAG]\
                    ,[OFFERINGFLAG]\
                    ,[RECDATETIME]\
                    ,[UNIX]\
                    ,[MARKET]\
                    FROM ";
  }

  static std::string selCmd_STOCKSTATUS() {
    return "SELECT   [SECURITYID]\
                    ,[TRDDATE]\
                    ,[SECURITYPRENAME]\
                    ,[TRADINGPHASECODE]\
                    ,[CRDBUYSTATUS]\
                    ,[CRDSELLSTATUS]\
                    ,[SUBSCRIBESTATUS]\
                    ,[REDEMPTIONSTATUS]\
                    ,[WARRANTCREATED]\
                    ,[WARRANTDROPPED]\
                    ,[RECDATETIME]\
                    ,[UNIX]\
                    ,[MARKET]\
                    FROM ";
  }

  static std::string selCmd_TRADE() {
    return "SELECT   [SECURITYID]\
                    ,[TRDDATE]\
                    ,[TRADETIME]\
                    ,[SETNO]\
                    ,[RECNO]\
                    ,[BUYORDERRECNO]\
                    ,[SELLORDERRECNO]\
                    ,[TRADEPRICE]\
                    ,[TRADEQTY]\
                    ,[ORDERKIND]\
                    ,[FUNCTIONCODE]\
                    ,[RECDATETIME]\
                    ,[UNIX]\
                    ,[MARKET]\
                    FROM ";
  }


};


#endif
