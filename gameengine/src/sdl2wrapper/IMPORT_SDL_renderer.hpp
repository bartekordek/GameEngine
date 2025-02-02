#pragma once
#if _MSC_VER
#pragma warning( push, 0 )
#pragma warning( disable: 4514 )
#pragma warning( disable: 4820 )
#endif

#define SDL_MAIN_HANDLED
#include <SDL_render.h>
#define SDL_MAIN_HANDLED

#ifdef _MSC_VER
#pragma warning( pop )
#endif