#ifndef GTAADAPTER_H
#define GTAADAPTER_H

#include "CedarTimeHelper.h"
#include "CedarHelper.h"
#include "DataAdapter.h"
#include "ProtoBufMsgHub.h"
#include "stdlib.h"
#include "GTAHelper.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "json/json.h"

#include <unistd.h>
#include <sqlfront.h>
#include <sybdb.h>

template<class T>
class GTAAdapter : public DataAdapter {
  public:
  GTAAdapter() : oneDay(1), oneMonth(1) {
    columnNum = 69;
    dataType = "";
  };

  int init(std::string &stream, Json::Value &config) {

    LOGINREC *login = NULL;
    RETCODE erc;
    //  LOG(INFO) << config;
    loadConfig(config);
    streamName = stream;

    if (dbinit() == FAIL) {
      LOG(INFO) << "dbinit failed";
      return -1;
    }

    if ((login = dblogin()) == NULL) {
      LOG(INFO) << "login structure allocate failed";
      return -1;
    }

    DBSETLUSER(login, const_cast<char *>(userName.c_str()));
    DBSETLPWD(login, const_cast<char *>(passwd.c_str()));

    if (NULL ==
        (dbproc = dbopen(login, const_cast<char *>(serverAddr.c_str())))) {
      LOG(INFO) << "db open error";
      return -1;
    }

    ref = nextData.GetReflection();
    pDescriptor =
            google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(
                    dataType);
    columnNum = pDescriptor->field_count();
    if (NULL == ref || NULL == pDescriptor) {
      LOG(INFO)
              << "Problem encounted in reading config of Protobuf data structure";
    }
  }

  bool getNextData(TimeSeriesData &tsData) {
    while (true) {
      if (curTime > endTime) {
        break;
      }

      if (curTime.date().month() != dbTime.date().month()) {
        dbTime += oneMonth;
        if (!readDB()) {
          dbTime += oneMonth;
          continue;
        } else {
          LOG(INFO) << "readDB succeed!";
        }
      }

      int getResult = getNextDBData();
      if (getResult == 0) {
        continue;
      } else if (getResult == 1) {

        CedarMsgType type = getType();
        tsData.msg =
                ProtoBufHelper::toMessageBase<T>(type, nextData);
        tsData.ts = curTime;        //set timestamp to current time
        tsData.streamName = streamName;
        //LOG(INFO) << nextData.DebugString();
        return true;
      } else if (getResult == -1) {
        curTime += oneDay;
        LOG(INFO) << "let's go next day! " << curTime;
      }
    }

    return false;
  }

  int close() {
    dbclose(dbproc);
    dbexit();
  }

  T nextData;

  private:
  int loadConfig(Json::Value &config) {
    serverAddr = config["cassicDataServer"]["Addr"].asString();
    start = config["cassicDataServer"]["Start"].asString();
    end = config["cassicDataServer"]["End"].asString();
    userName = config["cassicDataServer"]["UserName"].asString();
    passwd = config["cassicDataServer"]["PassWD"].asString();
    code = config["code"].asString();
    dataType = config["dataType"].asString();

    std::string startStr = config["Start"].asString();
    std::string EndStr = config["End"].asString();

    startTime = CedarTimeHelper::strToPTime("%Y%m%d", startStr);
    endTime = CedarTimeHelper::strToPTime("%Y%m%d", EndStr);
    LOG(INFO) << dataType;
    curTime = startTime;
    dbTime = startTime - oneMonth;

    /*LOG(INFO) << "addr:" << serverAddr;
    LOG(INFO) << "start:" << start;
    LOG(INFO) << "end:" << end;
    LOG(INFO) << "userName:" << userName;
    LOG(INFO) << "passwd:" << passwd;*/
  }

  int getResult() {
    int errFlag = 0;
    if ((erc = dbresults(dbproc)) != NO_MORE_RESULTS) {

      for (int i = 0; i < columnNum; i++) {
        if (FAIL == dbbind(dbproc, i + 1, STRINGBIND, (DBINT) 0,
                           (BYTE *) (dataBuf[i]))) {
          LOG(INFO) << "fetch line " << i << " error!";
        }
      }
    }
    return 0;
  }

  CedarMsgType getType() {
    if (dataType == "TaqData") {
      return TYPE_TAQ_DATA_GTA;
    } else if (dataType == "IndexData") {
      return TYPE_INDEX_DATA_GTA;
    } else {
      return TYPE_DATAREQUEST;
    }
  }

  int readDB() {
    std::string dbName = GTAHelper::getDBName(dataType, code, dbTime);
    std::string tableName = GTAHelper::getTableName(dataType, code, dbTime);

    if (dbuse(dbproc, const_cast<char *>(dbName.c_str())) == FAIL) {
      LOG(INFO) << dbName << " not found";
      return -1;
    }

    std::string cmd = GTAHelper::getSelCmd(dataType, code, curTime);
    dbcmd(dbproc, const_cast<char *>(cmd.c_str()));

    if (FAIL == (erc = dbsqlexec(dbproc))) {
      LOG(INFO) << "searching " << dbName << " failed!";
      return -1;
    }

    getResult();

    return 1;
  }

  int getNextDBData() {
    if (dbnextrow(dbproc) == NO_MORE_ROWS) {
      LOG(INFO) << "no more data";
      return -1;
    }

    //LOG(INFO) << "PFD num is:" << columnNum;
    for (int i = 0; i < columnNum; i++) {
      pFD = pDescriptor->field(i);
      std::string sBuf = dataBuf[i];
      switch (pFD->type()) {
        case 9:                   //9 represents TYPE_STRING, instant number here because compile error
          ref->SetString(&nextData, pFD, sBuf);
          break;

        case 1:                   //1 represents TYPE_DOUBLE
          float dTmp;
          dTmp = atof(sBuf.c_str());
          //sscanf(const_cast<char*>(sBuf.c_str()),"%f",&dTmp);
          ref->SetDouble(&nextData, pFD, dTmp);
          break;
      }
    }

    std::string dateStr = nextData.trddate();
    curTime = CedarTimeHelper::strToPTime("%Y%m%d", dateStr);

    if (curTime < startTime) {
      return 0;
    }

    if (curTime > endTime) {
      return -2;
    }

    //LOG(INFO) << curTime << "," << endTime;
    return 1;
  }

  std::string serverAddr;
  std::string start;
  std::string end;
  std::string port;
  std::string userName;
  std::string passwd;
  std::string code;
  std::string streamName;
  std::string dataType;
  //T nextData;

  DBPROCESS *dbproc;
  const google::protobuf::Reflection *ref;
  const google::protobuf::Descriptor *pDescriptor;
  const google::protobuf::FieldDescriptor *pFD;
  //T nextData;

  int columnNum;
  static const int readLength = 1024;
  const int bufLength = 50;
  static const int fieldNum = 69;
  char dataBuf[fieldNum][readLength];
  boost::posix_time::ptime startTime, endTime, curTime, dbTime;
  boost::gregorian::days oneDay;
  boost::gregorian::months oneMonth;
  RETCODE erc;
};

#endif
