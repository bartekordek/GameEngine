#pragma once

#include "gameengine/Render/RendererTypes.hpp"
#include "gameengine/Windowing/WinData.hpp"

#include "CUL/Graphics/Pos2D.hpp"

NAMESPACE_BEGIN( LOGLW )

struct GAME_ENGINE_API EngineParams
{
    bool legacy = false;
    WinData WinDataVal;
    String ConfigPath;
};

NAMESPACE_END( LOGLW )