#include "MdAdapter.h"
#include <algorithm>
#include <iostream>

MdAdapter::MdAdapter() : filesize(0) {
}

void MdAdapter::mapData(std::string filepath) {
  /* LOG(INFO) << "mapdata from:" << filepath; */
  int fd = open(filepath.c_str(), O_RDONLY);

  struct stat fileStat;
  fstat(fd, &fileStat);
  filesize = fileStat.st_size;

  pMD = mmap(NULL, filesize, PROT_READ, MAP_SHARED, fd, 0);
  close(fd); 
}

void MdAdapter::unmapData() {
  munmap(pMD, filesize);
  totalTick = 0;
  tickIter = 0;
}

int MdAdapter::getTickIter() {
  return tickIter;
}

void MdAdapter::nextTick() {
  tickIter++;
}

void MdAdapter::inQueue(Json::Value root) {
  pqElem qe;
  std::string szCode = root.getMemberNames()[0];
  if(queueMap.find(szCode) == queueMap.end()) {
    std::queue<Json::Value> q;
    q.push(root);
    bufQueues.push_back(q);
    queueMap[szCode] = bufQueues.size() - 1;
  } else {
    bufQueues[queueMap[szCode]].push(root);  
  }
}

void MdAdapter::loadFile(std::string filepath, int type) { 
  std::ifstream filestream(filepath);
  /* if(iterRecord.find(filepath) == iterRecord.end()) */
  /*   iterRecord[filepath] = std::count(std::istreambuf_iterator<char>(filestream), std::istreambuf_iterator<char>(), '\n'); */

  std::string cur_line;
  Json::Reader reader;
  Json::Value root;
  do{
    std::getline(filestream, cur_line);
    if(!reader.parse(cur_line, root, false))
      break;
    inQueue(root);
  }while(1);
}

MessageBase WindMdAdapter::getCedarMsg() {
}

