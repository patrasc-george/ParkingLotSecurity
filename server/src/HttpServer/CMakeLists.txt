project(HttpServer)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

add_dependencies(${PROJECT_NAME} SubscriptionManager)

find_package(Poco REQUIRED COMPONENTS Foundation Net NetSSL Crypto)
find_package(nlohmann_json CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC 
	"${CMAKE_SOURCE_DIR}/src/SubscriptionManager"
	"${CMAKE_SOURCE_DIR}/src/WebSocketServer"
	"${CMAKE_SOURCE_DIR}/src/QRCodeDetection"
)

target_link_libraries(${PROJECT_NAME} 
	SubscriptionManager 
	WebSocketServer
	QRCodeDetection
	Poco::Net 
	Poco::NetSSL 
	Poco::Crypto
	nlohmann_json::nlohmann_json
)
