project(VehicleManager)

add_definitions(-DVEHICLEMANAGER_EXPORTS)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

add_dependencies(${PROJECT_NAME} 
    QRCodeDetection
    LicensePlateDetection
    WebSocketClient
)

target_include_directories(${PROJECT_NAME} PUBLIC 
    "${CMAKE_SOURCE_DIR}/src/QRCodeDetection"
    "${CMAKE_SOURCE_DIR}/src/LicensePlateDetection"
    "${CMAKE_SOURCE_DIR}/src/WebSocketClient"
)

target_link_libraries(${PROJECT_NAME} 
    QRCodeDetection
    LicensePlateDetection
    WebSocketClient
)
