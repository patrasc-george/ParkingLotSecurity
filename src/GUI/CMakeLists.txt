cmake_minimum_required(VERSION 3.0)
project(UserInterface)

# Define UI exports
add_definitions(-DUI_EXPORTS)

# Disable automatic UI compilation
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC ON)

# Set the root directory for the GUI
set(GUI_ROOT_DIR "${ROOT_DIR}/src/GUI")

# Collect all headers and sources in the directory
file(
    GLOB_RECURSE GUI_FILES
    LIST_DIRECTORIES false
    "${GUI_ROOT_DIR}/*.c*"
    "${GUI_ROOT_DIR}/*.h*"
)

# Add the library
add_library(${PROJECT_NAME} STATIC ${GUI_FILES})

# Add dependencies
add_dependencies(${PROJECT_NAME} ImageProcessingUtils)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_SOURCE_DIR}/src/ImageProcessingUtils"
    "${CMAKE_SOURCE_DIR}/src/GUI"
    ${OpenCV_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ImageProcessingUtils
    Qt6::Widgets 
    Qt6::Core
    ${OpenCV_LIBS}
)

# Link to the runtime output directory
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Install the target to the 'bin' directory on installation
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
