cmake_minimum_required(VERSION 3.16)
project(ParkingLotsSecurity)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Application)

find_package(OpenCV REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Gui Core Widgets)
find_package(Tesseract REQUIRED)

add_subdirectory(tests)
add_subdirectory(src/Application)
add_subdirectory(src/DatabaseManager)
add_subdirectory(src/ImageProcessingUtils)
add_subdirectory(src/QRCodeUtils)
add_subdirectory(src/VehicleManager)
add_subdirectory(src/GUI)
add_subdirectory(src/HttpServer)
