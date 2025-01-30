#pragma once

#include "CUL/GenericUtils/DisableErros.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mouse.h>
#include <SDL_surface.h>
#include <SDL_syswm.h>
#undef ABSOLUTE

#ifdef _MSC_VER
#pragma warning( pop )
#endif