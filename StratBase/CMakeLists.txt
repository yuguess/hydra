cmake_minimum_required(VERSION 2.8.9)

set(CMAKE_CXX_FLAGS "-std=c++0x -Wall")

include_directories(./include
  ../../CedarInclude/CTP
  ../CedarHelper/include
  ../ProtoBufMsgHub/include
  ../CedarJsonConfig/include
  ../Backtester/include
  ../Indicator/include
  ../ProtoBufMsg)

set(PROTOBUF ../ProtoBufMsg/CPlusPlusCode/ProtoBufMsg.pb.cc)
set(SOURCES src/StratBase.cpp src/OrderDelegate.cpp)

LINK_DIRECTORIES(../../CedarLib ../Backtester/build)

#add_executable(BasicRSI src/BasicRSIMain.cpp src/BasicRSI.cpp
#  ${SOURCES} ${PROTOBUF})
#TARGET_LINK_LIBRARIES(BasicRSI zmq thosttraderapi jsoncpp protobuf
#  pthread backtester boost_filesystem boost_system boost_date_time)

#add_executable(DualRSI src/DualRSIMain.cpp src/DualRSI.cpp
#  ${SOURCES} ${PROTOBUF})
#TARGET_LINK_LIBRARIES(DualRSI zmq jsoncpp protobuf
#  pthread backtester boost_filesystem boost_system boost_date_time)

add_executable(DualKD src/DualKD.cpp src/DualKDMain.cpp
  ${SOURCES} ${PROTOBUF})
TARGET_LINK_LIBRARIES(DualKD zmq jsoncpp protobuf
  pthread backtester boost_filesystem boost_system boost_date_time)

#add_executable(BasicKD src/BasicKDMain.cpp src/BasicKD.cpp
#  ${SOURCES} ${PROTOBUF})
#TARGET_LINK_LIBRARIES(BasicKD zmq jsoncpp protobuf
#  pthread backtester boost_filesystem boost_system boost_date_time)
