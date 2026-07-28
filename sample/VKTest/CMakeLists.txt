get_filename_component( basename ${CMAKE_CURRENT_SOURCE_DIR} NAME )
project( ${basename} LANGUAGES CXX)

set(PROJECT_NAME ${basename})

project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4)
    add_compile_options(/MP /Zc:__cplusplus)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
    add_definitions(-Wfatal-errors)
endif()

file(GLOB_RECURSE SHADER_SOURCES *.vert *.frag )
message("Found shader files:")
foreach(SHADER ${SHADER_SOURCES})
message(" - ${SHADER}")
endforeach()

get_filename_component(SHADER_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders ABSOLUTE)
message("Shader output directory: ${SHADER_OUTPUT_DIR}")
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})
set(SPIRV_BINARY_FILES)
foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(FILE_NAME ${SHADER} NAME)
    set(SPIRV "${SHADER_OUTPUT_DIR}/${FILE_NAME}.spv")

    message("Adding shader ${FILE_NAME}")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${GLSLC_EXECUTABLE} ${SHADER} -o ${SPIRV}
        DEPENDS ${SHADER}
        COMMENT "Compiling shader ${FILE_NAME}"
        VERBATIM
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach()
add_custom_target(Shaders DEPENDS ${SPIRV_BINARY_FILES})

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

find_program(GLSLC_EXECUTABLE
    NAMES glslc
    HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{VULKAN_SDK}/bin"
)

if(NOT GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc not found - make sure the Vulkan SDK is installed and VULKAN_SDK is set")
endif()


set( DEPENDENCIES
PRIVATE SDL2-static
PRIVATE Vulkan::Vulkan
)

if( MSVC )
    target_link_libraries( ${PROJECT_NAME} ${DEPENDENCIES} )
else()
    target_link_libraries( ${PROJECT_NAME} ${DEPENDENCIES} stdc++fs  )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE inc)
target_include_directories(${PROJECT_NAME} PRIVATE src)

if( MSVC )
    set_property(TARGET ${PROJECT_NAME} PROPERTY "VS_DEBUGGER_WORKING_DIRECTORY" "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "Samples")
add_dependencies(${PROJECT_NAME} Shaders)