project(SubscriptionManager)

add_definitions(-DACCOUNT_EXPORTS)
add_definitions(-DSUBSCRIPTION_EXPORTS)
add_definitions(-DSUBSCRIPTIONMANAGER_EXPORTS)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

add_dependencies(${PROJECT_NAME} DatabaseManager)

find_package(nlohmann_json CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src/DatabaseManager")

target_link_libraries(${PROJECT_NAME} 
	DatabaseManager
	nlohmann_json::nlohmann_json
)
