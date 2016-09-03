#ifndef JSON_HELPER_H
#define JSON_HELPER_H

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <vector>
#include "json/json.h"

class JsonHelper {
public:
  static bool loadJsonFile(std::string path, Json::Value &root) {
    std::ifstream fileStream(path, std::ifstream::binary);

    Json::Reader reader;
    if (!reader.parse(fileStream, root)) {
      std::cerr << "Load json file " << path << " error, "
        << "check your path or json file format";
      exit(-1);
    }

    return true;
  }

  static int getStringArrayWithTag(Json::Value &root, std::string arrayPath,
    std::string tagName, std::vector<std::string> &results) {

    Json::Value val;
    getJsonValueByPath(root, arrayPath, val);

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

  static const Json::Value& getJsonValueByPath(Json::Value &root,
    std::string path, Json::Value &val) {

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
};

#endif
