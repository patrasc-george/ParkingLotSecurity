project(Application)

# Add the executable
file(GLOB SOURCE_FILES "*.cpp")
file(GLOB HEADER_FILES "*.h")

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# Set executable properties
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Parking Lots Security Camera")

# Add dependencies to libraries from other subprojects
add_dependencies(${PROJECT_NAME}
    ImageProcessingUtils
    UserInterface
)

# Allow headers to be included in main.cpp
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_SOURCE_DIR}/src/ImageProcessingUtils"
    "${CMAKE_SOURCE_DIR}/src/GUI"
    ${OpenCV_INCLUDE_DIRS}
)

# Link to the required libraries
target_link_libraries(${PROJECT_NAME}
    ImageProcessingUtils
    UserInterface
    Qt6::Core
    Qt6::Widgets
    ${OpenCV_LIBS}
)

# Link to the runtime output directory
target_link_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Install the executable to the 'bin' directory on installation
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
