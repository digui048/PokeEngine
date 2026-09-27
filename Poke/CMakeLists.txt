cmake_minimum_required(VERSION 3.22)
project("PokeEngine" VERSION 0.1.0 LANGUAGES C CXX)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders")
set(OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${SRC_DIR}/*.cpp" "${SRC_DIR}/*.h")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

find_package(Vulkan REQUIRED MODULE)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(assimp REQUIRED)
find_package(DevIL REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(TIFF REQUIRED)
find_package(Jasper REQUIRED)
find_package(nlohmann_json REQUIRED)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_library(PokeEngineCore ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC "${SRC_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/..")

target_link_libraries(${PROJECT_NAME} PUBLIC 
    SDL3::SDL3-shared
    SDL3_image::SDL3_image-shared
    glm::glm
    imgui
    Vulkan::Vulkan
    spdlog::spdlog
    assimp::assimp
    DevIL::IL
    DevIL::ILU
    DevIL::ILUT
    PNG::PNG
    TIFF::TIFF
    Jasper::Jasper
    JPEG::JPEG
    nlohmann_json::nlohmann_json
)

find_program(GLSLC_EXECUTABLE glslc HINTS $ENV{VULKAN_SDK}/bin)

if(GLSLC_EXECUTABLE AND EXISTS "${SHADERS_DIR}")
    file(GLOB_RECURSE SHADER_SOURCES CONFIGURE_DEPENDS
        "${SHADERS_DIR}/*.vert"    
        "${SHADERS_DIR}/*.frag"    
    )
    
    set(SPIRV_BINARY_FILES "")

    foreach(SHADER_SRC ${SHADER_SOURCES})
        file(RELATIVE_PATH REL_PATH "${SHADERS_DIR}" "${SHADER_SRC}")
        set(SPIRV_OUT "${SHADERS_DIR}/${REL_PATH}.spv")

        get_filename_component(OUT_DIR "${SPIRV_OUT}" DIRECTORY)

        add_custom_command(
            OUTPUT "${SPIRV_OUT}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${OUT_DIR}"
            COMMAND ${GLSLC_EXECUTABLE} "${SHADER_SRC}" -o "${SPIRV_OUT}"
            DEPENDS "${SHADER_SRC}"
            COMMENT "Compiling shader: ${REL_PATH} -> ${REL_PATH}.spv"
        )

        list(APPEND SPIRV_BINARY_FILES "${SPIRV_OUT}")
    endforeach()

    add_custom_target(CompileShaders DEPENDS ${SPIRV_BINARY_FILES})
    add_dependencies(${PROJECT_NAME} CompileShaders)
else()
    message(WARNING "glslc wasn't found or the directory ${SHADERS_DIR} doesn't exist.")
endif()
    
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${OUTPUT_PATH}")
endif()