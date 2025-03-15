#pragma once

#include "gameengine/Import.hpp"
#include "CUL/String.hpp"

#include "CUL/STL_IMPORTS/STD_functional.hpp"

NAMESPACE_BEGIN( LOGLW )

class GAME_ENGINE_API IDebugOverlay
{
public:
    IDebugOverlay() = default;
    virtual ~IDebugOverlay() = default;
protected:
private:
    IDebugOverlay( const IDebugOverlay& arg ) = delete;
    IDebugOverlay( IDebugOverlay&& arg ) = delete;
    IDebugOverlay& operator=( const IDebugOverlay& arg ) = delete;
    IDebugOverlay& operator=( IDebugOverlay&& arg ) = delete;
};

NAMESPACE_END( LOGLW )