#cmake -DCMAKE_BUILD_TYPE=Debug
cmake_minimum_required(VERSION 3.0.0)

set(PROJECT_NAME_DYNAMIC gameengine_dynamic)
set(PROJECT_NAME_STATIC gameengine_static)

project(${PROJECT_NAME_STATIC} LANGUAGES CXX)
project(${PROJECT_NAME_DYNAMIC} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories( SYSTEM ${GLM_INCLUDE_DIR} )
include_directories( SYSTEM ${GLEW_INCLUDE_DIR} )
include_directories( SYSTEM ${GLFW_INCLUDE_DIR} )
include_directories( SYSTEM $ENV{freeglut_INCLUDE_DIR} )

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "${dir}")
endforeach()

if( MSVC )
    add_compile_options( /W4 )
    add_compile_options( /MP )
else()
    add_compile_options( -Wall -Wextra -pedantic -Werror )
    add_definitions( -Wfatal-errors )
endif()

find_package( GLEW REQUIRED )

file( GLOB_RECURSE SOURCES include/*.hpp src/*.hpp src/*.cpp )

source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

add_library( ${PROJECT_NAME_STATIC} STATIC ${SOURCES} )
add_library( ${PROJECT_NAME_DYNAMIC} SHARED ${SOURCES} )

set( DEPENDENCIES_STATIC
PUBLIC LOGLW-AdditionalDeps
PUBLIC sdl2wrapper_static
PRIVATE glew_s
PUBLIC glfw
PRIVATE freeglut_static
PRIVATE tinyobjloader
)

set( DEPENDENCIES_DYNAMIC
PUBLIC LOGLW-AdditionalDeps
PUBLIC sdl2wrapper_dynamic
PUBLIC glew
PUBLIC glfw
PRIVATE freeglut_static
PRIVATE tinyobjloader
)

message( "DEBUG: PROJECT_NAME_STATIC: ${PROJECT_NAME_STATIC}" )
message( "DEBUG: PROJECT_NAME_DYNAMIC: ${PROJECT_NAME_DYNAMIC}" )

if( MSVC )
    target_link_libraries( ${PROJECT_NAME_STATIC} ${DEPENDENCIES_STATIC} )
    target_link_libraries( ${PROJECT_NAME_DYNAMIC} ${DEPENDENCIES_DYNAMIC} )
else()
    target_link_libraries( ${PROJECT_NAME_STATIC} ${DEPENDENCIES_STATIC} stdc++fs  )
    target_link_libraries( ${PROJECT_NAME_DYNAMIC} ${DEPENDENCIES_DYNAMIC} stdc++fs  )
endif()

if( MSVC )

#copy glew
    if( ${CMAKE_BUILD_TYPE} MATCHES "Debug" )
        set( GLEW_BIN glew32d.dll )
    else()
        set( GLEW_BIN glew32.dll )
    endif()
    add_custom_command( TARGET ${PROJECT_NAME_DYNAMIC}
        POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bin/${CMAKE_BUILD_TYPE}/${GLEW_BIN} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE} )
endif()

target_include_directories(${PROJECT_NAME_DYNAMIC} PUBLIC include)
target_include_directories(${PROJECT_NAME_DYNAMIC} PRIVATE src)

target_include_directories(${PROJECT_NAME_STATIC} PUBLIC include)
target_include_directories(${PROJECT_NAME_STATIC} PRIVATE src)