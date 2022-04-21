#cmake -DCMAKE_BUILD_TYPE=Debug

get_filename_component( basename ${CMAKE_CURRENT_SOURCE_DIR} NAME )
project( ${basename} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(EXEC ${PROJECT_NAME})

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
    add_compile_options( /MP )
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
    add_definitions(-Wfatal-errors)
endif()

file( GLOB_RECURSE SOURCES *.hpp *.cpp )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

add_executable(${EXEC} ${SOURCES})

if (MSVC)
    set_target_properties(
        ${EXEC}
        PROPERTIES
            OUTPUT_NAME ${EXEC}
            SUFFIX ".exe"
    )
else()
    set_target_properties(
        ${EXEC}
        PROPERTIES
            OUTPUT_NAME ${EXEC}
            SUFFIX ".bin"
    )
endif()

IF(NOT APPLE AND NOT MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
ENDIF()


if( MSVC )
    target_link_libraries( ${EXEC} gameengine LOGLW-AdditionalDeps cul AdditionalDeps )
else()
    target_link_libraries( ${EXEC} gameengine LOGLW-AdditionalDeps cul AdditionalDeps stdc++fs  )
endif()

target_include_directories(${EXEC} PRIVATE inc)
target_include_directories(${EXEC} PRIVATE src)

if( MSVC )
    set_property(TARGET ${EXEC} PROPERTY "VS_DEBUGGER_WORKING_DIRECTORY" "${OGLW_TESTS_RUN_DIR}")
endif()