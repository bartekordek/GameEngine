#pragma once

#include "CUL/GenericUtils/DisableErros.hpp"
CUL_MSVC_SKIP_DEFAULT_WARNING_BEGIN

#include <SDL_opengl.h>
#include <SDL_opengl_glext.h>
#include <SDL_video.h>
#undef ABSOLUTE

#ifdef _MSC_VER
#pragma warning( pop )
#endif