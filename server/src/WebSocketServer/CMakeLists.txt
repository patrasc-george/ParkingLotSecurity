project(WebSocketServer)

cmake_policy(SET CMP0167 NEW)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

add_dependencies(${PROJECT_NAME} DatabaseManager)

find_package(websocketpp REQUIRED)
find_package(Boost REQUIRED COMPONENTS beast asio)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src/DatabaseManager")

target_link_libraries(${PROJECT_NAME} 
	DatabaseManager
    websocketpp::websocketpp
	Boost::boost 
	Boost::beast 
	Boost::asio
)
