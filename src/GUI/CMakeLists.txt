project(UserInterface)

# Define UI exports
add_definitions(-DUI_EXPORTS)

# Disable automatic UI compilation
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC ON)

set(TRANSLATIONS 
	"${CMAKE_SOURCE_DIR}/translations/gui_en.ts"
    "${CMAKE_SOURCE_DIR}/translations/gui_ro.ts"
	)

# Collect every header and source in the directory
file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.c*")

# Collect all headers and sources in the directory
file(
    GLOB_RECURSE GUI_FILES
    LIST_DIRECTORIES false
	${HEADER_FILES} 
	${SOURCE_FILES}
	)

# Add the library
add_library(${PROJECT_NAME} STATIC ${GUI_FILES})

# Add dependencies
add_dependencies(${PROJECT_NAME} ImageProcessingUtils)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_SOURCE_DIR}/src/ImageProcessingUtils"
    "${CMAKE_SOURCE_DIR}/src/GUI"
    ${OpenCV_INCLUDE_DIRS}
	)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ImageProcessingUtils
    Qt6::Widgets 
    Qt6::Core
    ${OpenCV_LIBS}
	)

# Link to the runtime output directory
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
