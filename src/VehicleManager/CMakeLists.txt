project(VehicleManager)

add_definitions(-DVEHICLEMANAGER_EXPORTS)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

add_dependencies(${PROJECT_NAME} 
	ImageProcessingUtils
	QRCodeUtils
	)

target_include_directories(${PROJECT_NAME} PUBLIC 
	"${CMAKE_SOURCE_DIR}/src/ImageProcessingUtils"
	"${CMAKE_SOURCE_DIR}/src/QRCodeUtils"
	)

target_link_libraries(${PROJECT_NAME} 
	ImageProcessingUtils
	QRCodeUtils
	)
