#cmake -DCMAKE_BUILD_TYPE=Debug
cmake_minimum_required(VERSION 3.0.0)

project(game_engine_main)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(OGLW_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(OGLW_BUILD_ROOT ${CMAKE_SOURCE_DIR})
set(OGLW_RES_DIR ${OGLW_ROOT}/res)
set(ENV{OGLW_RES_DIR} ${OGLW_ROOT}/res)

if(MSVC)
    set(BUILD_ROOT_POSTFIX Build-Windows)
else()
    set(BUILD_ROOT_POSTFIX Build-Linux)
endif()

set(OGLW_OUTPUT_DIR ${OGLW_BUILD_ROOT}/${BUILD_ROOT_POSTFIX})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OGLW_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OGLW_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OGLW_OUTPUT_DIR})

set(OGLW_TESTS_RUN_DIR ${OGLW_ROOT}/runDir)
set(OGLW_DEPS_ROOT ${OGLW_ROOT}/deps)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/deps/CUL/cmake_modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/deps")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

set(FREEGLUT_REPLACE_GLUT ON CACHE BOOL "REPLACE GLUT" FORCE)
set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "DO NOT BUILD DEMOS" FORCE)
set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "DO NOT BUILD SHARED" FORCE)
add_subdirectory(${OGLW_DEPS_ROOT}/freeglut-3.2.1)

# Add sub directories
set( SDL2_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL/include )
add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/deps/SDL )
add_subdirectory(deps)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/CUL)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/rapidobj)

find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW REQUIRED)
find_package(freeglut REQUIRED)

set(GLM_INCLUDE_DIR $ENV{GLM_INCLUDE_DIR})
set(GLM_INCLUDE_DIRS $ENV{GLM_INCLUDE_DIRS})

set(GLEW_HOME ${CMAKE_CURRENT_SOURCE_DIR}/deps/glew-2.1.0)
set(ENV{GLEW_HOME} ${GLEW_HOME})
set(GLFW_HOME ${CMAKE_CURRENT_SOURCE_DIR}/deps/glfw)
set(GLEW_INCLUDE_DIR ${GLEW_HOME}/include )
set(GLEW_INCLUDE_DIRS ${GLEW_HOME}/include)
set(GLFW_INCLUDE_DIR ${GLFW_HOME}/include)

list(APPEND CMAKE_MODULE_PATH ${GLEW_HOME})
set(GLFW_LIBRARY_TYPE STATIC CACHE STRING "BUILD STATIC" FORCE)
add_subdirectory(${GLEW_HOME}/build/cmake)
add_subdirectory(${GLFW_HOME})

set(TINYOBJLOADER_BUILD_TEST_LOADER OFF)
add_subdirectory(deps/tinyobjloader)

option(RENDER_OPENGL_CORE "Enable Opengl" ON)

add_subdirectory(gameengine)

if(MSVC)
    add_subdirectory(sample/dx12playground)
    add_subdirectory(sample/test00)
endif()

add_subdirectory(sample/playgroundApp)
add_subdirectory(sample/snake)
add_subdirectory(TemplateApp)
add_subdirectory(tools/shader_editor)