#pragma once

#include "gameengine/Import.hpp"

#include "SDL2Wrapper/WinSize.hpp"

#include "CUL/Graphics/Pos2D.hpp"

NAMESPACE_BEGIN( LOGLW )

struct GAME_ENGINE_API EngineParams
{
    bool legacy = false;
    CUL::Graphics::Pos2Di windowPosition;
    SDL2W::WinSize winSize;
    CUL::String winName;
    CUL::String rendererName;
    CUL::String configPath;
};

NAMESPACE_END( LOGLW )