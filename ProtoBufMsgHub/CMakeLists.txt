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
add_executable(protoBufMsgHub src/Main.cpp ${PROTOBUF})
TARGET_LINK_LIBRARIES(protoBufMsgHub zmq thosttraderapi
  jsoncpp protobuf pthread)

add_executable(testClient src/TestClient.cpp ${PROTOBUF})
TARGET_LINK_LIBRARIES(testClient zmq thosttraderapi
  jsoncpp protobuf pthread)
