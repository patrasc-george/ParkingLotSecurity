cmake_minimum_required(VERSION 3.10)
project(DatabaseManager)

add_definitions(-DDATABASEMANAGER_EXPORTS)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

find_package(sqlcipher REQUIRED)

target_compile_definitions(${PROJECT_NAME} PRIVATE -DSQLITE_HAS_CODEC)

target_link_libraries(${PROJECT_NAME} PRIVATE sqlcipher)
