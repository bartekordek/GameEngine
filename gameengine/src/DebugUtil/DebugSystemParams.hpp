#pragma once

#include "gameengine/Import.hpp"

struct SDL_Window;

NAMESPACE_BEGIN( LOGLW )

class IRenderDevice;

struct DebugSystemParams
{
    IRenderDevice* RenderDevice = nullptr;
    SDL_Window* SDLWindow = nullptr;
    void* SDL_GL_Context = nullptr;
    void* D3D12DevicePtr = nullptr;
};

NAMESPACE_END( LOGLW )