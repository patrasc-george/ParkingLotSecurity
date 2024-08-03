project(VehicleManager)

add_definitions(-DVEHICLEMANAGER_EXPORTS)

# Collect every header and source in the directory
file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

# Add the library
add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

# Add dependencies
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