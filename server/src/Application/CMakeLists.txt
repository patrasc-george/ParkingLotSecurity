project(Application)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

add_dependencies(${PROJECT_NAME} HttpServer)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Server")

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src/HttpServer")

target_link_libraries(${PROJECT_NAME} HttpServer)

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
