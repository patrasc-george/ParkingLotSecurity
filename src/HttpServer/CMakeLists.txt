project(HttpServer)

find_package(Poco REQUIRED COMPONENTS Net NetSSL Crypto)

add_definitions(-DHTTPSERVER_EXPORTS)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

add_dependencies(${PROJECT_NAME} VehicleManager)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src/VehicleManager")

target_link_libraries(${PROJECT_NAME} VehicleManager Poco::Net Poco::NetSSL Poco::Crypto)
