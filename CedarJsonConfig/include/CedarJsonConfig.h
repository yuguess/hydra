#ifndef CEDAR_JSON_CONFIG_H
#define CEDAR_JSON_CONFIG_H

#include <stdio.h>
//#include "rapidjson/document.h"
//#include "rapidjson/filereadstream.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <vector>
#include "CedarHelper.h"

#ifdef __linux
	#include "json/reader.h"
    #include "json/writer.h"
#elif  _WIN32
	#include "json/json.h"	
#endif

class CedarJsonConfig {
public:
  static CedarJsonConfig& getInstance() {
    static CedarJsonConfig jsonConfig;
    return jsonConfig;
  }

  int loadConfigFile(std::string path) {
    std::ifstream fileStream(path, std::ifstream::binary);

    Json::Reader reader;
    if (!reader.parse(fileStream, root)) {
      LOG(FATAL) << "Parse config file " << path << " error, "
        << "check your path or config file format";
    }

	return 0;
  }

  int getStringByPath(std::string path, std::string &result) {
    Json::Value val;
    getJsonValueByPath(path, val);
    if (!val.isString())
      LOG(FATAL) << "Get string failed, check your valuetype " << path;

    result = val.asString();

    return 0;
  }

  int getIntByPath(std::string path, int &result) {
    Json::Value val;
    getJsonValueByPath(path, val);
    if (!val.isInt())
      LOG(FATAL) << "Get string failed, check your valuetype " << path;

    result = val.asInt();

    return 0;
  }
  //arrayPath is the 
  int getStringArrayWithTag(std::vector<std::string> &results, 
      std::string arrayPath, std::string tagName = "") {

    Json::Value val;
    getJsonValueByPath(arrayPath, val);

    if (!val.isArray())
      LOG(FATAL) << "Get array failed, check your array path" << arrayPath;

    for (unsigned int i = 0; i < val.size(); i++) {
      std::string tmp; 
      if (tagName == "") {
        if (val[i].isString()) 
          tmp = val[i].asString();
        else
          LOG(FATAL) << "array path " << arrayPath 
            <<  " seems wrong, please check";

      } else {
        if (val[i].isMember(tagName) && val[i][tagName].isString()) {
          tmp = val[i][tagName].asString();
        } else {
          LOG(FATAL) << "tag " << tagName << " seems wrong, please check";
        }
      }

      results.push_back(tmp);

    }
    return 0;
  }

  const Json::Value& getJsonValueByPath(std::string path, Json::Value &val) {
    if (path == "")
      LOG(FATAL) << "Input path is empty, Check !!!!";

    std::vector<std::string> strs;
    boost::split(strs, path, boost::is_any_of("."));

    if ((val = root[strs[0]]) == Json::nullValue) {
      LOG(FATAL) << "Input path is empty, Check !!!!";
    }

    if (strs.size() == 1)
      return val;

    //for nested path only
    for (unsigned int i = 1; i < strs.size() - 1; i++) {
      if ((val = val[strs[i]]) == Json::nullValue)
        LOG(FATAL) << "Tag " << strs[i] << " has error !";
    }

    if ((val = val[strs[strs.size() - 1]]) == Json::nullValue) {
      LOG(FATAL) << "Tag " << strs[strs.size() - 1] << " has error !";
    }

    return val;
  }

  std::string valueToString(const Json::Value val) {
    return fastWriter.write(val);    
  }

  //int loadConfigFile(std::string path) {
  //  FILE *fp = NULL;
  //  if ((fp = fopen(path.c_str(), "r")) == NULL)
  //    LOG(FATAL) << "config file " << path << " does not exist";

  //  char readBuffer[BUFFER_MAX];
  //  rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
  //  doc.ParseStream(is);
  //  fclose(fp);

  //  if (!doc.IsObject())
  //    LOG(FATAL) << "config file parse failed, check your config file format";

  //  filePath = path;
  //  return 0;
  //}

  //int getStringByPath(std::string path, std::string &result) {
  //  rapidjson::Value val;
  //  getJsonValueByPath(path, val);
  //  if (!val.IsString())
  //    LOG(FATAL) << "get failed, check your path " << path;

  //  result = val.GetString();

  //  return 0;
  //}

  //int getStringArrayWithTag(std::string arrayPath, std::string tagName, 
  //    std::vector<std::string> &results) {

  //  rapidjson::Value val;
  //  getJsonValueByPath(arrayPath, val);

  //  if (!val.IsArray())
  //    LOG(FATAL) << "Get array failed, check your array path" << arrayPath;

  //  for (int i = 0; i < val.Size(); i++) {
  //    if (val[i].HasMember(tagName.c_str()) 
  //        && val[i][tagName.c_str()].IsString()) {

  //      results.push_back(val[i][tagName.c_str()].GetString());
  //    } else {
  //      LOG(FATAL) << "tag " << tagName << " seems wrong, please check";
  //    }
  //  }

  //  return 0;
  //}

  //int getJsonValueByPath(std::string path, rapidjson::Value &val) {
  //  if (path == "")
  //    LOG(FATAL) << "Input path is empty, Check !!!!";

  //  std::vector<std::string> strs;
  //  boost::split(strs, path, boost::is_any_of("."));

  //  if (!doc.HasMember(strs[0].c_str()))
  //    LOG(FATAL) << "tag " << strs[0] << " seems wrong, please check";

  //  val = doc[strs[0].c_str()];
  //  if (strs.size() == 1) {
  //    loadConfigFile(filePath);
  //    return 0;
  //  }

  //  //for nested path only
  //  for (int i = 1; i < strs.size() - 1; i++) {
  //    if (!val.HasMember(strs[i].c_str()))
  //      LOG(FATAL) << "Tag " << strs[i] << " has error !";

  //    val = val[strs[i].c_str()];
  //  }

  //  if (val.HasMember(strs[strs.size() - 1].c_str()))  {
  //    val = val[strs[strs.size() - 1].c_str()];
  //  } else
  //    LOG(FATAL) << "Tag " << strs[strs.size() - 1] << " has error !";

  //  //TODO what hack here !!!!!!!!!!!!
  //  loadConfigFile(filePath);
  //  return 0;
  //}

private:
  Json::Value root;
  Json::FastWriter fastWriter;
  //const static int BUFFER_MAX = 125536;
  //rapidjson::Document doc;
  //std::string filePath;
};

#endif
