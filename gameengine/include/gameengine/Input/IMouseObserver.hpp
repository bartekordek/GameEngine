#pragma once

#include "gameengine/Import.hpp"

NAMESPACE_BEGIN( LOGLW )

class MouseData;

class GAME_ENGINE_API IMouseObserver
{
public:
    IMouseObserver() = default;
    virtual void onMouseEvent( const MouseData& md ) = 0;

    virtual ~IMouseObserver() = default;

protected:
private:
    IMouseObserver( const IMouseObserver& arg ) = delete;
    IMouseObserver( IMouseObserver&& arg ) = delete;
    IMouseObserver& operator=( const IMouseObserver& arg ) = delete;
    IMouseObserver& operator=( IMouseObserver&& arg ) = delete;
};

NAMESPACE_END( LOGLW )