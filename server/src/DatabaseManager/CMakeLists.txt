project(DatabaseManager)

add_definitions(-DDATABASEMANAGER_EXPORTS)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

find_package(PostgreSQL REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${PostgreSQL_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE PostgreSQL::PostgreSQL)
