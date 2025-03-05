project(Application)

file(GLOB HEADER_FILES "*.h")
file(GLOB SOURCE_FILES "*.cpp")

add_executable(${PROJECT_NAME} WIN32 ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "ParkingLotsSecurityCamera")

add_dependencies(${PROJECT_NAME} Interface)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src/Interface")

target_link_libraries(${PROJECT_NAME} Interface)

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(DELIVERY_DIR "${CMAKE_SOURCE_DIR}/../deliverypack")

install(TARGETS ${PROJECT_NAME} DESTINATION "${DELIVERY_DIR}")
install(FILES $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> DESTINATION "${DELIVERY_DIR}")

set(Qt6_DLLS "${Qt6_DIR}/../../../plugins")
install(DIRECTORY ${Qt6_DLLS} DESTINATION "${DELIVERY_DIR}" USE_SOURCE_PERMISSIONS)

file(GLOB Tesseract_DLLS "${Tesseract_INCLUDE_DIRS}/../bin/*.dll")
install(FILES ${Tesseract_DLLS} DESTINATION "${DELIVERY_DIR}")

install(FILES ${QRENCODE_LIBRARIES} DESTINATION "${DELIVERY_DIR}")

file(GLOB ASSETS 
    "${CMAKE_SOURCE_DIR}/assets/*.png" 
    "${CMAKE_SOURCE_DIR}/assets/I.jpg" 
    "${CMAKE_SOURCE_DIR}/assets/vehicles.txt"
    "${CMAKE_SOURCE_DIR}/assets/subscriptions.txt"
)
install(FILES ${ASSETS} DESTINATION "${DELIVERY_DIR}/assets")

file(MAKE_DIRECTORY "${DELIVERY_DIR}/database/vehicles")
file(MAKE_DIRECTORY "${DELIVERY_DIR}/database/tickets")

file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/database/vehicles")
file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/database/tickets")

file(GLOB TRANSLATIONS "${CMAKE_SOURCE_DIR}/translations/*.qm")
install(FILES ${TRANSLATIONS} DESTINATION "${DELIVERY_DIR}/translations")
