#cmake -DCMAKE_BUILD_TYPE=Debug

get_filename_component( basename ${CMAKE_CURRENT_SOURCE_DIR} NAME )
project( ${basename} LANGUAGES CXX)

set(PROJECT_NAME_DYNAMIC ${basename}_dynamic)
set(PROJECT_NAME_STATIC ${basename}_static)

project(${PROJECT_NAME_STATIC} LANGUAGES CXX)
project(${PROJECT_NAME_DYNAMIC} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
    add_compile_options( /MP )
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
    add_definitions(-Wfatal-errors)
endif()

file( GLOB_RECURSE SOURCES *.hpp *.cpp *.txt )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

add_executable(${PROJECT_NAME_DYNAMIC} ${SOURCES})
add_executable(${PROJECT_NAME_STATIC} ${SOURCES})

if (MSVC)
    set_target_properties(
        ${PROJECT_NAME_DYNAMIC}
        PROPERTIES
            OUTPUT_NAME ${PROJECT_NAME_DYNAMIC}
            SUFFIX ".exe"
    )
else()
    set_target_properties(
        ${PROJECT_NAME_DYNAMIC}
        PROPERTIES
            OUTPUT_NAME ${PROJECT_NAME_DYNAMIC}
            SUFFIX ".bin"
    )
endif()

IF(NOT APPLE AND NOT MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
ENDIF()

set( DEPENDENCIES_STATIC
PRIVATE gameengine_static
PRIVATE AdditionalDeps
)

set( DEPENDENCIES_DYNAMIC
PRIVATE gameengine_dynamic
PRIVATE AdditionalDeps
)

if( MSVC )
    target_link_libraries( ${PROJECT_NAME_DYNAMIC} ${DEPENDENCIES_DYNAMIC} )
    target_link_libraries( ${PROJECT_NAME_STATIC} ${DEPENDENCIES_STATIC} )
else()
    target_link_libraries( ${PROJECT_NAME_DYNAMIC} ${DEPENDENCIES_DYNAMIC} stdc++fs  )
    target_link_libraries( ${PROJECT_NAME_STATIC} ${DEPENDENCIES_STATIC} stdc++fs  )
endif()

target_include_directories(${PROJECT_NAME_DYNAMIC} PRIVATE inc)
target_include_directories(${PROJECT_NAME_DYNAMIC} PRIVATE src)

target_include_directories(${PROJECT_NAME_STATIC} PRIVATE inc)
target_include_directories(${PROJECT_NAME_STATIC} PRIVATE src)

if( MSVC )
    set_property(TARGET ${PROJECT_NAME_DYNAMIC} PROPERTY "VS_DEBUGGER_WORKING_DIRECTORY" "${CMAKE_CURRENT_SOURCE_DIR}")
    set_property(TARGET ${PROJECT_NAME_STATIC} PROPERTY "VS_DEBUGGER_WORKING_DIRECTORY" "${CMAKE_CURRENT_SOURCE_DIR}")
endif()