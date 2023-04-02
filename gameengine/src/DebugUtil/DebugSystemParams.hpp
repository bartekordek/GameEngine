#pragma once

#include "gameengine/Import.hpp"

struct SDL_Window;

NAMESPACE_BEGIN( LOGLW )

struct DebugSystemParams
{
	SDL_Window* SDLWindow = nullptr;
	void* SDL_GL_Context = nullptr;
};

NAMESPACE_END( LOGLW )