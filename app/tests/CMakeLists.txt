project(Tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SOURCE_FILES "*.cpp")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC 
    ${OpenCV_INCLUDE_DIRS}
    ${Tesseract_INCLUDE_DIRS}
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src/LicensePlateDetection")

target_link_libraries(${PROJECT_NAME} PUBLIC
    LicensePlateDetection
    ${OpenCV_LIBS}
    ${Tesseract_LIBRARIES}
    ${Tesseract_LIBRARY_DEBUG}
)

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_test(
    NAME ${PROJECT_NAME}
    COMMAND vstest.console.exe Tests.dll
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIGURATION>
)
