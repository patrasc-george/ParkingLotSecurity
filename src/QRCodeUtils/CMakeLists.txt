project(QRCodeUtils)

add_definitions(-DQRCODEUTILS_EXPORTS)

# Collect every header and source in the directory
file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

# Add the library
add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC 
	${QRENCODE_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
	)

target_link_libraries(${PROJECT_NAME} PUBLIC 
	${QRENCODE_LIBRARIES}
    ${OpenCV_LIBS}
	)

find_path(QRENCODE_INCLUDE_DIR NAMES qrencode.h)
find_library(QRENCODE_LIBRARY_RELEASE qrencode)
find_library(QRENCODE_LIBRARY_DEBUG qrencoded)

set(QRENCODE_LIBRARIES optimized ${QRENCODE_LIBRARY_RELEASE} debug ${QRENCODE_LIBRARY_DEBUG})
