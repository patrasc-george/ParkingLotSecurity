project(WebSocketServer)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

add_dependencies(${PROJECT_NAME} DatabaseManager)

find_package(PostgreSQL REQUIRED)
find_package(Boost REQUIRED COMPONENTS beast asio)
find_package(nlohmann_json CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC 
	"${CMAKE_SOURCE_DIR}/src/DatabaseManager"
	"${CMAKE_SOURCE_DIR}/src/Logger"
	${PostgreSQL_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} 
	DatabaseManager
	Logger
    PostgreSQL::PostgreSQL
	Boost::boost 
	Boost::beast 
	Boost::asio
	nlohmann_json::nlohmann_json
)
