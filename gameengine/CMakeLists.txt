#cmake -DCMAKE_BUILD_TYPE=Debug
cmake_minimum_required(VERSION 3.0.0)

set(PROJECT_NAME_DYNAMIC gameengine_dynamic)
set(PROJECT_NAME_STATIC gameengine_static)

project(${PROJECT_NAME_STATIC} LANGUAGES CXX)
project(${PROJECT_NAME_DYNAMIC} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
cmake_policy(SET CMP0069 NEW)

include_directories( SYSTEM ${GLEW_INCLUDE_DIR} )
include_directories( SYSTEM ${GLM_INCLUDE_DIR} )
include_directories( SYSTEM ${OGLW_DEPS_ROOT}/CUL/libcul/inc )
include_directories( SYSTEM ${OGLW_DEPS_ROOT}/SDL/include )
include_directories( SYSTEM $ENV{freeglut_INCLUDE_DIR} )

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "${dir}")
endforeach()

if( MSVC )
    add_compile_options( /Wall )
    add_compile_options( /MP )

    add_compile_options("$<$<CONFIG:RelWithDebInfo>:/Ot>")
    add_compile_options("$<$<CONFIG:RelWithDebInfo>:/Oi>")

    add_compile_options("$<$<CONFIG:Release>:/Ot>")
    add_compile_options("$<$<CONFIG:Release>:/Oi>")
else()
    add_compile_options(-Wall -Wextra -pedantic)
    add_definitions( -Wfatal-errors )
endif()

find_package( GLEW REQUIRED )

file( GLOB_RECURSE SOURCES */*.hpp */*.cpp */*/*.hpp */*/*.cpp */*.frag */*.vert )

source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

add_library( ${PROJECT_NAME_STATIC} STATIC ${SOURCES} )
add_library( ${PROJECT_NAME_DYNAMIC} SHARED ${SOURCES} )

set_property( TARGET ${PROJECT_NAME_STATIC} PROPERTY INTERPROCEDURAL_OPTIMIZATION_DEBUG false )
set_property( TARGET ${PROJECT_NAME_STATIC} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO true )
set_property( TARGET ${PROJECT_NAME_STATIC} PROPERTY INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL true )
set_property( TARGET ${PROJECT_NAME_STATIC} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE true )

set_property( TARGET ${PROJECT_NAME_DYNAMIC} PROPERTY INTERPROCEDURAL_OPTIMIZATION_DEBUG false )
set_property( TARGET ${PROJECT_NAME_DYNAMIC} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO true )
set_property( TARGET ${PROJECT_NAME_DYNAMIC} PROPERTY INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL true )
set_property( TARGET ${PROJECT_NAME_DYNAMIC} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE true )

if(Vulkan_FOUND)
    find_program(GLSLC_EXECUTABLE
        NAMES glslc
        HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{VULKAN_SDK}/bin"
    )

    if(NOT GLSLC_EXECUTABLE)
        message(FATAL_ERROR "glslc not found - make sure the Vulkan SDK is installed and VULKAN_SDK is set")
    endif()

    file(GLOB_RECURSE SHADER_SOURCES vk_shaders/*.vert vk_shaders/*.frag )
    message("Found VK shader files:")
    foreach(SHADER ${SHADER_SOURCES})
        message(" - ${SHADER}")
    endforeach()

    get_filename_component(SHADER_OUTPUT_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bin ABSOLUTE)
    message("Shader output directory: ${SHADER_OUTPUT_DIR}")

    set(SPIRV_BINARY_FILES)
    foreach(SHADER ${SHADER_SOURCES})
        get_filename_component(FILE_NAME ${SHADER} NAME)
        set(SPIRV "${SHADER_OUTPUT_DIR}/${FILE_NAME}.spv")

        message("GE: Adding shader ${FILE_NAME}")
        add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSLC_EXECUTABLE} ${SHADER} -o ${SPIRV}
            DEPENDS ${SHADER}
            COMMENT "Compiling shader ${FILE_NAME}"
            VERBATIM
        )
        list(APPEND SPIRV_BINARY_FILES ${SPIRV})
    endforeach()
    add_custom_target(VulkanShaders DEPENDS ${SPIRV_BINARY_FILES})

    add_dependencies(${PROJECT_NAME_STATIC} VulkanShaders)
    add_dependencies(${PROJECT_NAME_DYNAMIC} VulkanShaders)
endif()

set( DEPENDENCIES_STATIC
PUBLIC LOGLW-AdditionalDeps
PUBLIC cul_static
PUBLIC glew_s
PUBLIC freeglut_static
PUBLIC SDL2-static
PRIVATE rapidobj
)

set( DEPENDENCIES_DYNAMIC
PUBLIC LOGLW-AdditionalDeps
PUBLIC cul_dynamic
PUBLIC glew
PUBLIC freeglut_static
PUBLIC SDL2
PRIVATE rapidobj
)

if(Vulkan_FOUND)
message("Vulkan found.")
list(APPEND DEPENDENCIES_STATIC PRIVATE Vulkan::Vulkan)
list(APPEND DEPENDENCIES_DYNAMIC PRIVATE Vulkan::Vulkan)
target_compile_definitions(${PROJECT_NAME_STATIC} PRIVATE VULKAN_ENABLED=1)
target_compile_definitions(${PROJECT_NAME_DYNAMIC} PRIVATE VULKAN_ENABLED=1)
else()
message("Vulkan not found.")
target_compile_definitions(${PROJECT_NAME_STATIC} PRIVATE VULKAN_ENABLED=0)
target_compile_definitions(${PROJECT_NAME_DYNAMIC} PRIVATE VULKAN_ENABLED=0)
endif()

if( MSVC )
    target_link_libraries( ${PROJECT_NAME_STATIC} ${DEPENDENCIES_STATIC} )
    target_link_libraries( ${PROJECT_NAME_DYNAMIC} ${DEPENDENCIES_DYNAMIC} )
else()
    target_link_libraries( ${PROJECT_NAME_STATIC} ${DEPENDENCIES_STATIC} stdc++fs  )
    target_link_libraries( ${PROJECT_NAME_DYNAMIC} ${DEPENDENCIES_DYNAMIC} stdc++fs  )
endif()

if( MSVC )
    add_custom_command( TARGET ${PROJECT_NAME_DYNAMIC}
        POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/bin/$<CONFIG>" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>" )
endif()

target_include_directories(${PROJECT_NAME_DYNAMIC} PUBLIC include)
target_include_directories(${PROJECT_NAME_DYNAMIC} PRIVATE src)

target_include_directories(${PROJECT_NAME_STATIC} PUBLIC include)
target_include_directories(${PROJECT_NAME_STATIC} PRIVATE src)