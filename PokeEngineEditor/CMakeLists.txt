cmake_minimum_required(VERSION 3.22)
project("PokeEngineEditor" VERSION 0.1.0 LANGUAGES C CXX)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.h")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC "${SRC_DIR}")

target_link_libraries(${PROJECT_NAME} PUBLIC PokeEngineCore)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${OUTPUT_PATH}")
endif()