cmake_minimum_required(VERSION 3.16)
project(Server)

set(CMAKE_CXX_VISIBILITY_PRESET "default")
set(CMAKE_VISIBILITY_INLINES_HIDDEN "YES")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Application)

add_subdirectory(src/DatabaseManager)
add_subdirectory(src/SubscriptionManager)
add_subdirectory(src/WebSocketServer)
add_subdirectory(src/QRCodeDetection)
add_subdirectory(src/HttpServer)
add_subdirectory(src/Application)
