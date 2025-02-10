project(QRCodeDetection)

add_definitions(-DQRCODEDETECTION_EXPORTS)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

find_package(OpenCV REQUIRED)
find_path(QRENCODE_INCLUDE_DIR NAMES qrencode.h)
find_library(QRENCODE_LIBRARY_RELEASE qrencode)
find_library(QRENCODE_LIBRARY_DEBUG qrencoded)

target_include_directories(${PROJECT_NAME} PUBLIC 
	"${CMAKE_SOURCE_DIR}/src/Logger"
    ${OpenCV_INCLUDE_DIRS}
	${QRENCODE_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PUBLIC 
	Logger
    ${OpenCV_LIBS}
	${QRENCODE_LIBRARIES}
)

set(QRENCODE_LIBRARIES optimized ${QRENCODE_LIBRARY_RELEASE} debug ${QRENCODE_LIBRARY_DEBUG})
