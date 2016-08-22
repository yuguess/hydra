#ifndef PROTOBUF_MSG_HUB_H
#define PROTOBUF_MSG_HUB_H

#include <iostream>
#include <vector>
#include <zmq.h>
#include <thread>
#include <functional>
#include <unordered_map>
#include <memory>

#include "CPlusPlusCode/ProtoBufMsg.pb.h"
#include "CedarJsonConfig.h"
#include "easylogging++.h"

class ProtoBufMsgHub {

public:
  ProtoBufMsgHub() : zmqCtx(NULL), pubSock(NULL),
    subSock(NULL), pullSock(NULL), closeFlag(true) {
  }

  ~ProtoBufMsgHub() {
    closeFlag = false;
    if (pubSock != NULL)
      zmq_close(pubSock);
    if (subSock != NULL)
      zmq_close(subSock);
    if (pullSock != NULL)
      zmq_close(pullSock);
    if (zmqCtx != NULL)
      zmq_ctx_destroy(zmqCtx);

    for (auto it = pullerAddrs.begin(); it != pullerAddrs.end(); it++)
      zmq_close(it->second.get());
  }

  typedef std::function<int(MessageBase)> MsgCallback;

  int init(std::string &pub, std::string &pull) {
    pubAddr = "tcp://*:" + pub;
    pullAddr = "tcp://*:" + pull;

    if ((zmqCtx = zmq_ctx_new()) == NULL)
       LOG(FATAL) << "zmq_ctx_new error: " << std::string(zmq_strerror(errno));

    if ((pubSock = zmq_socket(zmqCtx, ZMQ_PUB)) == NULL)
       LOG(FATAL) << "zmq_socket error: " << std::string(zmq_strerror(errno));
    if ((subSock = zmq_socket(zmqCtx, ZMQ_SUB)) == NULL)
      LOG(FATAL) << "zmq_socket error: " << std::string(zmq_strerror(errno));
    if ((pullSock = zmq_socket(zmqCtx, ZMQ_PULL)) == NULL)
      LOG(FATAL) << "zmq_socket error: " << std::string(zmq_strerror(errno));

    int optval = 1;
    zmq_setsockopt(pubSock, ZMQ_TCP_KEEPALIVE, &optval, 4);

    if (zmq_bind(pubSock, pubAddr.c_str()) != 0)
       LOG(FATAL) << "zmq_bind error: " << std::string(zmq_strerror(errno));

    if (zmq_bind(pullSock, pullAddr.c_str()) != 0)
       LOG(FATAL) << "zmq_bind error: " << std::string(zmq_strerror(errno));

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    subThr = std::thread(&ProtoBufMsgHub::poll, this);
    subThr.detach();

    return 0;
  }

  int registerCallback(MsgCallback callback) {
    msgCallback = callback;
    return 0;
  }

  int boardcastMsg(std::string channel, std::string str) {
    str = channel + "|" + str;
    if (zmq_send(pubSock, str.c_str(), str.size(), ZMQ_DONTWAIT)
        != str.size()) {
      //if (errno == EAGAIN)
      //  LOG(ERROR) << "zmq_send EAGAIN" << std::endl;
      LOG(ERROR) << std::string(zmq_strerror(errno)) << std::endl;
      return -1;
    }
    return 0;
  }

  int addSubscription(std::string addr, std::string channel) {
    if (publisherAddrs.find(addr) == publisherAddrs.end()) {
      std::string subAddr = "tcp://" + addr;
      if (zmq_connect(subSock, subAddr.c_str()) != 0) {
         LOG(ERROR) << "zmq_connect error:" << std::string(zmq_strerror(errno));
         return -1;
      }
      publisherAddrs[addr] = true;
    }
    //send data request

    zmq_setsockopt(subSock, ZMQ_SUBSCRIBE, channel.c_str(), channel.size());

    return 0;

    //for (unsigned i = 0; i < subscribers.size(); i++) {
    //  std::string pushAddr = "tcp://" + subscribers[i].hostPort;
    //  LOG(INFO) << "subscribe address:" << pushAddr << std::endl;

    //  int optval = 1;
    //  zmq_setsockopt(pubSock, ZMQ_TCP_KEEPALIVE, &optval, 4);
    //  if (zmq_connect(subSock, pushAddr.c_str()) != 0) {
    //     LOG(ERROR) << "zmq_connect error:"
    //       << std::string(zmq_strerror(errno)) << std::endl;
    //  }

    //  std::string channel = subscribers[i].name;
    //  zmq_setsockopt(subSock, ZMQ_SUBSCRIBE, channel.c_str(), channel.size());
    //}
    //return 0;
  }

  int pushMsg(std::string addr, std::string msg) {
    // check if addr exist if it does use it else create it;
    void *pushSockPtr = NULL;
    std::shared_ptr<void> pushSock;

    if (pullerAddrs.find(addr) == pullerAddrs.end()) {
      if ((pushSockPtr = zmq_socket(zmqCtx, ZMQ_PUSH)) == NULL)
        LOG(ERROR) << "zmq_socket error: " << std::string(zmq_strerror(errno));
      std::string pullerAddr = "tcp://" + addr;
      if (zmq_connect(pushSockPtr, pullerAddr.c_str()) != 0) {
         LOG(ERROR) << "zmq_connect error:" << std::string(zmq_strerror(errno));
         return -1;
      }

      pushSock = std::shared_ptr<void>(pushSockPtr);
      pullerAddrs[addr] = pushSock;

    } else {
      pushSock = pullerAddrs[addr];
    }

    if (zmq_send(pushSock.get(), msg.c_str(), msg.size(), ZMQ_DONTWAIT)
        != msg.size()) {
      LOG(ERROR) << std::string(zmq_strerror(errno));
      return -1;
    }

    //LOG(INFO) << "PushMsg complete";

    return 0;
  }

  int close() {
    closeFlag = false;
    return 0;
  }

private:

  int removeChannelHeader(std::string &subMsg) {
    size_t pos = subMsg.find_first_of("|");
    //make sure the broken string doesn't crash the msghub
    if (pos == std::string::npos || pos -1 < 0 || pos + 1 > (subMsg.size() - 1)) {
      LOG(ERROR) << "recv invalid msg " << subMsg;
      return -1;
    }

    //chan = subMsg.substr(0, pos);
    subMsg = subMsg.substr(pos + 1);

    return 0;
  }

  int recvMsg(void *sock, bool isSubscribeMsg) {
    int status = 0;
    zmq_msg_t zmsg;
    int64_t more;
    size_t more_size = sizeof more;
    do {
      if (zmq_msg_init(&zmsg) != 0)
        LOG(ERROR) << std::string(zmq_strerror(errno));

      if ((status = zmq_msg_recv(&zmsg, sock, 0)) == -1) {
        if (errno == ETERM)  {
          LOG(INFO) << "ETERM, receiving thread exit";
          exit(-1);
        } else {
          LOG(ERROR) << "zmq_msg_recv error:" << std::string(zmq_strerror(errno));
          return -1;
        }
      }

      std::string msg((const char*)zmq_msg_data(&zmsg), status);

      if (isSubscribeMsg)
        removeChannelHeader(msg);

      MessageBase msgBase;
      if (!msgBase.ParseFromString(msg))
        LOG(ERROR) << "recv invalid msg " << msg;

      try {
        msgCallback(msgBase);
      } catch (const std::bad_function_call &e) {
        LOG(ERROR) << "recv msg but msghub doesn't have register callback";
        LOG(ERROR) << e.what();
      } catch (...) {
        LOG(ERROR) << "Msghub callback function error !";
      }

      if ((status = zmq_getsockopt(sock, ZMQ_RCVMORE, &more, &more_size)) != 0)
         LOG(ERROR) << std::string(zmq_strerror(errno));

    } while (more);

    zmq_msg_close(&zmsg);

    return 0;
  }

  int poll() {

    zmq_pollitem_t items[] = {
      {pullSock, 0, ZMQ_POLLIN, 0},
      {subSock, 0, ZMQ_POLLIN, 0}
    };

    int itemSize = sizeof(items) / sizeof(zmq_pollitem_t);
    while (closeFlag) {
      //LOG(INFO) << "block on zmq_poll1";

      if (zmq_poll(items, itemSize, -1) == -1) {
        if (errno == ETERM) {
          LOG(INFO) << "pool return for one sock become invalid";
        } else {
          LOG(ERROR) << zmq_strerror(errno);
        }
        exit(-1);
      }

      if (items[0].revents & ZMQ_POLLIN) {
        //LOG(INFO) << "pullSock get event";
        recvMsg(pullSock, false);
      }

      if (items[1].revents & ZMQ_POLLIN) {
        //LOG(INFO) << "subSock get event";
        recvMsg(subSock, true);
      }
    }

    return 0;
  }

  void *zmqCtx;
  void *pubSock;
  void *subSock;
  void *pullSock;

  std::map<std::string, bool> publisherAddrs;
  std::unordered_map<std::string, std::shared_ptr<void>> pullerAddrs;

  std::thread subThr;
  std::string pullPort;
  std::string pubAddr;
  std::string pullAddr;
  bool closeFlag;

  MsgCallback msgCallback;
};

class ProtoBufHelper {
public:
  template<typename T>
  static T unwrapMsg(MessageBase msgBase) {
    T protoBufObj;
    if (!protoBufObj.ParseFromString(msgBase.msg()))
      LOG(ERROR) << "recv invalid msg " << msgBase.msg();
    return protoBufObj;
  }

  template<typename T>
  static MessageBase toMessageBase(CedarMsgType msgType, T &obj) {
    std::string str;
    obj.SerializeToString(&str);

    MessageBase msgBase;
    msgBase.set_type(msgType);
    msgBase.set_msg(str);

    return msgBase;
  }

  template<typename T>
  static std::string wrapMsg(CedarMsgType msgType, T &obj) {
    std::string str;
    obj.SerializeToString(&str);

    MessageBase msgBase;
    msgBase.set_type(msgType);
    msgBase.set_msg(str);

    std::string baseMsgStr;
    msgBase.SerializeToString(&baseMsgStr);
    return baseMsgStr;
  }

  //need to call loadConfig to use this func usually in the beginning of
  //the main function
  static int setupProtoBufMsgHub(ProtoBufMsgHub &msgHub) {
    std::string pubPort;
    std::string pullPort;

    CedarJsonConfig::getInstance().getStringByPath("MsgHub.PublishPort", pubPort);
    CedarJsonConfig::getInstance().getStringByPath("MsgHub.PullPort", pullPort);

    msgHub.init(pubPort, pullPort);
    return 0;
  }
};

#endif
