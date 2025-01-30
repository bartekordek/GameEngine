#pragma once

#include "CUL/UselessMacros.hpp"

#if _WIN32
#define SDL2W_WINDOWS
#define SDL2W_EXPORT
#else
#define SDL2W_LINUX
#define SDL2W_EXPORT
#endif

#if defined SDL2W_EXPORT && defined SDL2W_LINUX
#define SDL2WAPI
#define SDL2Wrapper_TEMPLATE
#elif defined SDL2W_EXPORT && defined SDL2W_WINDOWS
#define SDL2WAPI _declspec(dllexport)
#define SDL2Wrapper_TEMPLATE extern
#else
#define SDL2WAPI _declspec(dllimport)
#define SDL2Wrapper_TEMPLATE extern
#endif
/*TODO: Find a way to tell library name.
#ifdef _MSC_VER && SDL2Wrapper_DYNAMIC
#pragma comment(lib, "SDL2WrapperDynamic.lib")
#elif  MSC_VER
#pragma comment(lib, "SDL2WrapperStatic.lib")
#else
#pragma comment(lib, "libsdl2wrapper.a")
#endif
*/