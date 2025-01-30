#cmake -DCMAKE_BUILD_TYPE=Debug
cmake_minimum_required(VERSION 3.0.0)

set(PROJECT_NAME_DYNAMIC sdl2wrapper_dynamic)
set(PROJECT_NAME_STATIC sdl2wrapper_static)

project(sdl2wrapper_static LANGUAGES CXX)
project(sdl2wrapper_dynamic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

if( MSVC )
    add_compile_options( /Wall )
    add_compile_options( /MP )
    add_compile_options("/wd4100")
    add_compile_options("/wd4266")
    add_compile_options("/wd4514")
    add_compile_options("/wd4710")
    add_compile_options("/wd4711")
    add_compile_options("/wd4820")
    # set_target_properties(SDL2 PROPERTIES
    #     COMPILE_FLAGS "/MP" # Adjust as needed
    # )
else()
    add_compile_options( -Wall -Wextra -pedantic -Werror -fPIC )
    add_definitions( -Wfatal-errors )
endif()

find_package( glm REQUIRED )
find_package( cul REQUIRED )
find_package( sdl2 REQUIRED )

file( GLOB_RECURSE SOURCES inc/*.hpp src/*.hpp src/*.cpp )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

add_library( ${PROJECT_NAME_DYNAMIC} SHARED ${SOURCES} )
add_library( ${PROJECT_NAME_STATIC} STATIC ${SOURCES} )

set( DEPENDENCIES_DYNAMIC
PUBLIC cul_dynamic
PUBLIC SDL2main
)

set( DEPENDENCIES_STATIC
PUBLIC cul_static
PUBLIC SDL2main
)

if( MSVC )
    set( DEPENDENCIES_WINDOWS_DYNAMIC ${DEPENDENCIES_DYNAMIC} PUBLIC SDL2 )
    set( DEPENDENCIES_WINDOWS_STATIC ${DEPENDENCIES_STATIC} PUBLIC SDL2-static )
else()
    set( DEPENDENCIES_LINUX_DYNAMIC ${DEPENDENCIES_DYNAMIC} PUBLIC SDL2 )
    set( DEPENDENCIES_LINUX_STATIC ${DEPENDENCIES_STATIC} PUBLIC SDL2-static )
endif()


set_property( TARGET ${PROJECT_NAME_DYNAMIC} PROPERTY "LINK_LIBRARY_DEPENDENCIES" true )
set_property( TARGET ${PROJECT_NAME_STATIC} PROPERTY "LINK_LIBRARY_DEPENDENCIES" true )

if( MSVC )
    message( "DEBUG: ${PROJECT_NAME_DYNAMIC} dependencies: ${DEPENDENCIES_WINDOWS} " )
    target_link_libraries( ${PROJECT_NAME_DYNAMIC} ${DEPENDENCIES_WINDOWS_DYNAMIC} )
    target_link_libraries( ${PROJECT_NAME_STATIC} ${DEPENDENCIES_WINDOWS_STATIC} )
else()
    target_link_libraries( ${PROJECT_NAME_DYNAMIC} ${DEPENDENCIES_LINUX_DYNAMIC} stdc++fs )
    target_link_libraries( ${PROJECT_NAME_STATIC} ${DEPENDENCIES_LINUX_STATIC} stdc++fs )
endif()

target_include_directories(${PROJECT_NAME_DYNAMIC} PUBLIC inc)
target_include_directories(${PROJECT_NAME_DYNAMIC} PRIVATE src)


target_include_directories(${PROJECT_NAME_STATIC} PUBLIC inc)
target_include_directories(${PROJECT_NAME_STATIC} PRIVATE src)

if (MSVC)
    set_target_properties(${EXEC} PROPERTIES LINK_FLAGS "/INCREMENTAL:NO")
endif()
