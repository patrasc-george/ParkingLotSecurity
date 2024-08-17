project(UserInterface)

add_definitions(-DUI_EXPORTS)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC ON)

set(TRANSLATIONS 
	"${CMAKE_SOURCE_DIR}/translations/gui_en.ts"
    "${CMAKE_SOURCE_DIR}/translations/gui_ro.ts"
	)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.c*")

file(
    GLOB_RECURSE GUI_FILES
    LIST_DIRECTORIES false
	${HEADER_FILES} 
	${SOURCE_FILES}
	)

add_library(${PROJECT_NAME} STATIC ${GUI_FILES})

add_dependencies(${PROJECT_NAME} VehicleManager)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_SOURCE_DIR}/src/VehicleManager"
    "${CMAKE_SOURCE_DIR}/src/GUI"
	)

target_link_libraries(${PROJECT_NAME}
    VehicleManager
    Qt6::Widgets 
    Qt6::Core
	)

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
