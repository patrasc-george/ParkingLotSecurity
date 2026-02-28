project(QRCodeDetection)

add_definitions(-DQRCODEDETECTION_EXPORTS)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

find_package(OpenCV REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${OpenCV_LIBS}
)
