#cmake -DCMAKE_BUILD_TYPE=Debug
cmake_minimum_required(VERSION 3.0.0)

project(LOGLW-AdditionalDeps LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(EXEC LOGLW-AdditionalDeps)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../deps/SDL2Wrapper/deps/)
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../deps/sdl2-repo ${CMAKE_CURRENT_BINARY_DIR}/../deps/sdl2-repo)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../deps/SDL2Wrapper/deps/sdl2-repo/include)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${FREEGLUT_HOME}" )
message( "[gameengine] 00 CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}" )
find_package( OpenGL REQUIRED )
find_package( GLEW REQUIRED )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OGLW_OUTPUT_DIR} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OGLW_OUTPUT_DIR} )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OGLW_OUTPUT_DIR} )

add_library(${EXEC} STATIC
imgui/backends/imgui_impl_opengl2.cpp
#imgui/examples/imgui_impl_opengl3.cpp
imgui/backends/imgui_impl_sdl.cpp
#imgui/examples/imgui_impl_win32.cpp
imgui/imgui.cpp
imgui/imgui_demo.cpp
imgui/imgui_draw.cpp
imgui/imgui_tables.cpp
imgui/imgui_widgets.cpp
imgui/misc/cpp/imgui_stdlib.cpp
imgui/misc/fonts/binary_to_compressed_c.cpp
#imgui/misc/freetype/imgui_freetype.cpp
)

set(HEADERS_PUBLIC
imgui/imconfig.h
imgui/imgui.h
imgui/imgui_internal.h
imgui/imstb_rectpack.h
imgui/imstb_textedit.h
imgui/imstb_truetype.h
imgui/misc/cpp/imgui_stdlib.h
#imgui/misc/freetype/imgui_freetype.h
)

target_include_directories(${EXEC} PUBLIC imgui)

#set(LIBS ${LIBS} ${OPENGL_LIBRARY} ${SDL2_LIBRARIES} ${GLEW_LIBRARIES} ${Assimp_LIBRARIES} )
set(LIBS ${GLEW_LIBRARIES} )

if (MSVC)
    target_link_libraries( ${EXEC} ${DEPENDENCIES} )
else()
    target_link_libraries( ${EXEC} ${DEPENDENCIES} stdc++fs )
endif()