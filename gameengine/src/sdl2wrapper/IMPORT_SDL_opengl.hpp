#pragma once

#include "CUL/GenericUtils/DisableErros.hpp"

#define SDL_MAIN_HANDLED
#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>
#include <SDL_video.h>
#define SDL_MAIN_HANDLED
#undef ABSOLUTE

#ifdef _MSC_VER
#pragma warning( pop )
#endif