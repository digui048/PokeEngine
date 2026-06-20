cmake_minimum_required(VERSION 3.22)
project("PokeEngine" VERSION 0.1.0 LANGUAGES C CXX)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

find_package(SDL3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.h")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_library(Poke-Engine-Core ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC "${SRC_DIR}")

target_link_libraries(${PROJECT_NAME} PUBLIC
    SDL3::SDL3
    glad::glad
    glm::glm
    imgui::imgui
)