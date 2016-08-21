cmake_minimum_required(VERSION 2.8.9)

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall")

include_directories(./include
  ../../CedarInclude/CTP
  ../CedarHelper/include
  ../ProtoBufMsgHub/include
  ../CedarJsonConfig/include
  ../ProtoBufMsg)

set(PROTOBUF ../ProtoBufMsg/CPlusPlusCode/ProtoBufMsg.pb.cc)

LINK_DIRECTORIES(../../CedarLib)
add_executable(CedarHelper src/Main.cpp ${PROTOBUF})
TARGET_LINK_LIBRARIES(CedarHelper zmq thosttraderapi jsoncpp protobuf)
