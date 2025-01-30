#pragma once

#include "SDL2Wrapper/Import.hpp"

union SDL_Event;

NAMESPACE_BEGIN( SDL2W )

class SDL2WAPI ISDLEventObserver
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

NAMESPACE_END( SDL2W )