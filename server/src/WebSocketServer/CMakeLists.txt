project(WebSocketServer)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

add_dependencies(${PROJECT_NAME} DatabaseManager)

find_package(PostgreSQL REQUIRED)
find_package(Boost REQUIRED COMPONENTS beast asio)

target_include_directories(${PROJECT_NAME} PUBLIC 
	${PostgreSQL_INCLUDE_DIRS}
	"${CMAKE_SOURCE_DIR}/src/DatabaseManager"
)

target_link_libraries(${PROJECT_NAME} 
	DatabaseManager
    PostgreSQL::PostgreSQL
	Boost::boost 
	Boost::beast 
	Boost::asio
)
