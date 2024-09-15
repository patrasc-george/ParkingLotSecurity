project(DatabaseManager)

add_definitions(-DDATABASEMANAGER_EXPORTS)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

find_package(unofficial-sqlite3 CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::sqlite3::sqlite3)