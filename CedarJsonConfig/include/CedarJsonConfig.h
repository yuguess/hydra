#ifndef CEDAR_JSON_CONFIG_H
#define CEDAR_JSON_CONFIG_H

#include <stdio.h>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <vector>

#include "easylogging++.h"

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
      std::cerr << "Load config file " << path << " error, "
        << "check your path or config file format";
      exit(-1);
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

  bool hasMember(std::string pathStr) {
    if (pathStr.size() == 0)
      return false;

    std::vector<std::string> sections;
    boost::split(sections, pathStr, boost::is_any_of("."));

    Json::Value node = root;
    unsigned i = 0;

    for (;i < sections.size(); i++) {
      if (node.isMember(sections[i])) {
        node = node[sections[i]];

        //go into the array object
        if (node.isArray() && node.isValidIndex(0))
          node = node[0];

        //reaching the "leaf" node
        if (!node.isObject()) {
          i++;
          break;
        }
      } else {
        return false;
      }
    }

    //i is from 0
    if (i == sections.size())
      return true;

    return false;
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
      LOG(FATAL) << "Input path is empty!";

    std::vector<std::string> strs;
    boost::split(strs, path, boost::is_any_of("."));

    if ((val = root[strs[0]]) == Json::nullValue) {
      LOG(FATAL) << "Input path " << strs[0] << " doest not exist,"
        << "Check path " << path;
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

private:
  Json::Value root;
  Json::FastWriter fastWriter;
};

#endif
