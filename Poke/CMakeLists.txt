cmake_minimum_required(VERSION 3.22)
project("PokeEngine" VERSION 0.1.0 LANGUAGES C CXX)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.h")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(glad REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)
find_package(OpenGL REQUIRED)
find_package(spdlog REQUIRED)
find_package(assimp REQUIRED)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_library(PokeEngineCore ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC "${SRC_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/..")

target_link_libraries(${PROJECT_NAME} PUBLIC 
    SDL3::SDL3
    SDL3_image::SDL3_image
    glad::glad
    glm::glm
    imgui::imgui
    OpenGL::GL
    spdlog::spdlog
    assimp::assimp
)