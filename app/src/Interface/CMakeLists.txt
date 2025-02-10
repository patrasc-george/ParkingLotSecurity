project(Interface)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC ON)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.c*")
file(
    GLOB_RECURSE INTERFACE_FILES
    LIST_DIRECTORIES false
    ${HEADER_FILES} 
    ${SOURCE_FILES}
)

add_library(${PROJECT_NAME} STATIC ${INTERFACE_FILES})

add_dependencies(${PROJECT_NAME} VehicleManager)

find_package(Qt6 REQUIRED COMPONENTS Gui Core Widgets)

target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_SOURCE_DIR}/src/Logger"
    "${CMAKE_SOURCE_DIR}/src/VehicleManager"
    "${CMAKE_SOURCE_DIR}/src/Interface"
)

target_link_libraries(${PROJECT_NAME}
	Logger
    VehicleManager
    Qt6::Widgets 
    Qt6::Core
)

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
