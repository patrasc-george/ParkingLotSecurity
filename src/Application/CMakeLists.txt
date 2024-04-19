project(Application)

# Add the executable
file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

# Set executable properties
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "ParkingLotsSecurityCamera")

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
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

install(TARGETS ${PROJECT_NAME} DESTINATION "${CMAKE_SOURCE_DIR}/DeliveryPack")
install(FILES $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> DESTINATION "${CMAKE_SOURCE_DIR}/DeliveryPack")

set(Qt6_DLLS "${Qt6_DIR}/../../../plugins")
install(DIRECTORY ${Qt6_DLLS} DESTINATION "${CMAKE_SOURCE_DIR}/DeliveryPack" USE_SOURCE_PERMISSIONS)

file(GLOB Tesseract_DLLS "${Tesseract_INCLUDE_DIRS}/../bin/*.dll")
install(FILES ${Tesseract_DLLS} DESTINATION "${CMAKE_SOURCE_DIR}/DeliveryPack")

file(GLOB DATABASE 
	"${CMAKE_SOURCE_DIR}/database/*.png" 
	"${CMAKE_SOURCE_DIR}/database/I.jpg" 
	"${CMAKE_SOURCE_DIR}/database/vehicles.txt"
	)
install(FILES ${DATABASE} DESTINATION "${CMAKE_SOURCE_DIR}/DeliveryPack/database")
