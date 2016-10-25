cmake_minimum_required(VERSION 2.8.9)

set(APP LtsMarketUpdate)

project(${APP})

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall")

include_directories(./include
  ../CedarHelper/include
  ../ProtoBufMsgHub/include
  ../CedarJsonConfig/include
  ../ProtoBufMsg)

set(PROTOBUF ../ProtoBufMsg/CPlusPlusCode/ProtoBufMsg.pb.cc)
file(GLOB SOURCES "src/*.cpp")

LINK_DIRECTORIES(./lib ../../CedarLib)
add_executable(${APP} ${SOURCES} ${PROTOBUF})
TARGET_LINK_LIBRARIES(${APP} zmq jsoncpp protobuf pthread securitymduserapi)
