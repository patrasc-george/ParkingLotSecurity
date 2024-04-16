project(ImageProcessingUtils)

# Define IMAGEPROCESSINGUTILS_EXPORTS
add_definitions(-DIMAGEPROCESSINGUTILS_EXPORTS)

# Collect every header and source in the directory
file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

# Add the library
add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES})

# Link against OpenCV and Tesseract
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${OpenCV_INCLUDE_DIRS}
    ${Tesseract_INCLUDE_DIRS}
	)

target_link_libraries(${PROJECT_NAME} PUBLIC
    ${OpenCV_LIBS}
    ${Tesseract_LIBRARIES}
	${Tesseract_LIBRARY_DEBUG}
	)
