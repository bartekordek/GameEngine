#pragma once


#include <CUL/CUL.hpp>
#include "CUL/UselessMacros.hpp"

#if _WIN32
#define GAME_ENGINE_WINDOWS
#define GAME_ENGINE_EXPORT
#else
#define GAME_ENGINE_LINUX
#define GAME_ENGINE_EXPORT
#endif

#if defined GAME_ENGINE_EXPORT && defined GAME_ENGINE_LINUX
#define GAME_ENGINE_API
#define GAME_ENGINE_POST
#define GAME_ENGINE_TEMPLATE
#elif defined GAME_ENGINE_EXPORT && defined GAME_ENGINE_WINDOWS
#define GAME_ENGINE_API __declspec(dllexport)
#define GAME_ENGINE_POST __cdecl
#define GAME_ENGINE_TEMPLATE
#else
#define GAME_ENGINE_API __declspec(dllimport)
#define GAME_ENGINE_POST __cdecl
#define GAME_ENGINE_TEMPLATE extern
#endif

#if _MSC_VER
#if defined(_M_X64) || defined(__amd64__)
#pragma warning( disable: 4820 )
#endif
#endif