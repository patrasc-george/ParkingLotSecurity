project(Application)

# Add the executable
file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_executable(${PROJECT_NAME} WIN32 ${HEADER_FILES} ${SOURCE_FILES})

# Set executable properties
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "ParkingLotsSecurityCamera")

# Add dependencies to libraries from other subprojects
add_dependencies(${PROJECT_NAME} UserInterface)

# Allow headers to be included in main.cpp
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src/GUI")

# Link to the required libraries
target_link_libraries(${PROJECT_NAME} UserInterface)

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
file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/DeliveryPack/database/vehicles")
file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/DeliveryPack/database/qr")
file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/database/vehicles")
file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/database/qr")

file(GLOB TRANSLATIONS "${CMAKE_SOURCE_DIR}/translations/*.qm" )
install(FILES ${TRANSLATIONS} DESTINATION "${CMAKE_SOURCE_DIR}/DeliveryPack/translations")
