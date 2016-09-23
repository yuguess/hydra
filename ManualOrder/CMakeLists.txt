cmake_minimum_required(VERSION 2.8.9)

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall")

include_directories(./include
  ../../CedarInclude/CTP
  ../CedarHelper/include
  ../ProtoBufMsgHub/include
  ../CedarJsonConfig/include
  ../ProtoBufMsg)

set(PROTOBUF ../ProtoBufMsg/CPlusPlusCode/ProtoBufMsg.pb.cc)
set(MANUAL_ORDER_SRC src/Main.cpp src/ManualOrder.cpp)

LINK_DIRECTORIES(../../CedarLib)

add_executable(Testcase src/Testcase.cpp ${PROTOBUF})
TARGET_LINK_LIBRARIES(Testcase  zmq jsoncpp protobuf pthread)

add_executable(ManualOrder ${MANUAL_ORDER_SRC} ${PROTOBUF})
TARGET_LINK_LIBRARIES(ManualOrder zmq jsoncpp protobuf pthread)
