cmake_minimum_required(VERSION 2.8.9)

set(APP hydra)

project(${APP})

add_subdirectory(ProtoBufMsgHub)
add_subdirectory(CedarHelper)
add_subdirectory(CTPTrade)
add_subdirectory(SmartOrderService)
add_subdirectory(StratBase)
