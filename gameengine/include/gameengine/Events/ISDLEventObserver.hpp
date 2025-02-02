#pragma once

#include "gameengine/Import.hpp"

union SDL_Event;

NAMESPACE_BEGIN( LOGLW )

class GAME_ENGINE_API ISDLEventObserver
{
public:
    ISDLEventObserver() = default;

    virtual void handleEvent( const SDL_Event& event ) = 0;

    virtual ~ISDLEventObserver() = default;

protected:
private:
    ISDLEventObserver( const ISDLEventObserver& arg ) = delete;
    ISDLEventObserver( ISDLEventObserver&& arg ) = delete;
    ISDLEventObserver& operator=( const ISDLEventObserver& rhv ) = delete;
    ISDLEventObserver& operator=( ISDLEventObserver&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )