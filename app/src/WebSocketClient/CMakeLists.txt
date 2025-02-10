project(WebSocketClient)

cmake_policy(SET CMP0167 NEW)

add_definitions(-DWEBSOCKETCLIENT_EXPORTS)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS beast asio)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src/Logger")
	
target_link_libraries(${PROJECT_NAME} PRIVATE 
	Logger
    nlohmann_json::nlohmann_json
    Boost::boost 
    Boost::beast 
    Boost::asio
)
