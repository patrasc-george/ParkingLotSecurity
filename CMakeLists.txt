cmake_minimum_required(VERSION 3.16)
project(ParkingLotsSecurity)

# Specify the output directories for executables and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Set the root directory
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Set the startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Application)

# Find required libraries
find_package(OpenCV REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Gui Core Widgets)
find_package(Tesseract REQUIRED)

# Add subdirectories for different parts of the application
add_subdirectory(tests)
add_subdirectory(src/Application)
add_subdirectory(src/ImageProcessingUtils)
add_subdirectory(src/QRCodeUtils)
add_subdirectory(src/VehicleManager)
add_subdirectory(src/GUI)
