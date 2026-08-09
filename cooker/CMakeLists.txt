#cmake -DCMAKE_BUILD_TYPE=Debug

get_filename_component( basename ${CMAKE_CURRENT_SOURCE_DIR} NAME )
project( ${basename} LANGUAGES CXX)

set(PROJECT_NAME ${basename})

project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

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

add_executable(${PROJECT_NAME} ${SOURCES})

if (MSVC)
    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
            OUTPUT_NAME ${PROJECT_NAME}
            SUFFIX ".exe"
    )
else()
    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
            OUTPUT_NAME ${PROJECT_NAME}
            SUFFIX ".bin"
    )
endif()

IF(NOT APPLE AND NOT MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
ENDIF()

set( DEPENDENCIES PRIVATE cul_static)

if( MSVC )
    target_link_libraries( ${PROJECT_NAME} ${DEPENDENCIES} )
else()
    target_link_libraries( ${PROJECT_NAME} ${DEPENDENCIES} stdc++fs  )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE inc)
target_include_directories(${PROJECT_NAME} PRIVATE src)

if( MSVC )
    get_filename_component(basePakPath
            "${CMAKE_SOURCE_DIR}/media/BasePak.json"
            ABSOLUTE
    )
    set_property(TARGET ${PROJECT_NAME} PROPERTY "VS_DEBUGGER_COMMAND_ARGUMENTS" "-jsonPath \"${basePakPath}\"")

    set_property(TARGET ${PROJECT_NAME} PROPERTY "VS_DEBUGGER_WORKING_DIRECTORY" "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "utils")