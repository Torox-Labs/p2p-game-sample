cmake_minimum_required(VERSION 3.21)
project(cube_test CXX)

# ====== Set C++ Standard ======
set(CMAKE_CXX_STANDARD 14)

# ====== Source and Include Directories ======
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

file(GLOB_RECURSE SOURCES
        "${SOURCE_DIR}/*.cpp"
        "${INCLUDE_DIR}/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})

# ====== Set Rox Engine Build Path ======
set(ROX_ENGINE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../rox-engine")
set(ROX_ENGINE_BUILD_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../rox-engine-build")

# ====== Include Directories =====lear=
target_include_directories(${PROJECT_NAME} PRIVATE
        ${ROX_ENGINE_PATH}/Engine/RoxCore
        ${ROX_ENGINE_PATH}/Engine/RoxGraphics
)

# ====== Include Libraries ======
# Force Debug mode for now since that's where your libraries are built
set(CMAKE_BUILD_TYPE "Debug")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ROX_LIB_DIR "${ROX_ENGINE_BUILD_PATH}/lib/Debug")
else()
    set(ROX_LIB_DIR "${ROX_ENGINE_BUILD_PATH}/lib/Release")
endif()

# Debug output
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "ROX_LIB_DIR: ${ROX_LIB_DIR}")
message(STATUS "ROX_ENGINE_BUILD_PATH: ${ROX_ENGINE_BUILD_PATH}")

# ====== Find all libraries in the directory ======
if(UNIX OR APPLE)
    file(GLOB ROX_LIBRARIES "${ROX_LIB_DIR}/*.a" "${ROX_LIB_DIR}/*.so" "${ROX_LIB_DIR}/*.dylib")
elseif(WIN32 OR MSVC)
    file(GLOB ROX_LIBRARIES "${ROX_LIB_DIR}/*.lib" "${ROX_LIB_DIR}/*.dll")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

# ====== Link Libraries ======
target_link_libraries(${PROJECT_NAME} PRIVATE
        # Main engine library first
        ${ROX_LIB_DIR}/librox_engine.so

        # High-level libraries
        ${ROX_LIB_DIR}/libRoxApp.a
        ${ROX_LIB_DIR}/libRoxScene.a
        ${ROX_LIB_DIR}/libRoxInput.a

        # Mid-level libraries
        ${ROX_LIB_DIR}/libRoxRender.a
        ${ROX_LIB_DIR}/libRoxResources.a
        ${ROX_LIB_DIR}/libRoxFormats.a
        ${ROX_LIB_DIR}/libRoxSystem.a

        # Core libraries last
        ${ROX_LIB_DIR}/libRoxMath.a
        ${ROX_LIB_DIR}/libRoxMemory.a
        ${ROX_LIB_DIR}/libRoxLogger.a

        # System libraries
        pthread
        dl
        GL
        X11
)

# ====== Copy Resources ======
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
)