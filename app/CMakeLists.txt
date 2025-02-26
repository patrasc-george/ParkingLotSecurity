cmake_minimum_required(VERSION 3.16)
project(ParkingLotsSecurity)

add_definitions(-D_WIN32_WINNT=0x0A00)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Application)

add_subdirectory(tests)
add_subdirectory(src/Logger)
add_subdirectory(src/QRCodeDetection)
add_subdirectory(src/LicensePlateDetection)
add_subdirectory(src/WebSocketClient)
add_subdirectory(src/VehicleManager)
add_subdirectory(src/Interface)
add_subdirectory(src/Application)
