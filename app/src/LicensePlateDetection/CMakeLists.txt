project(LicensePlateDetection)

add_definitions(-DLICENSEPLATEDETECTION_EXPORTS)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

find_package(OpenCV REQUIRED)
find_package(Tesseract REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_SOURCE_DIR}/src/Logger"
    ${OpenCV_INCLUDE_DIRS}
    ${Tesseract_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
	Logger
    ${OpenCV_LIBS}
    ${Tesseract_LIBRARIES}
    ${Tesseract_LIBRARY_DEBUG}
)
