#pragma once

#include "gameengine/Import.hpp"

NAMESPACE_BEGIN( LOGLW )

class GAME_ENGINE_API IEventLoop
{
public:
    IEventLoop() = default;

    /* Should be run in main process. */
    virtual void runEventLoop() = 0;
    virtual void stopEventLoop() = 0;

    virtual void pollEvents() = 0;

    virtual ~IEventLoop() = default;

protected:
private:
    IEventLoop( const IEventLoop& arg ) = delete;
    IEventLoop( IEventLoop&& arg ) = delete;
    IEventLoop& operator=( const IEventLoop& arg ) = delete;
    IEventLoop& operator=( IEventLoop&& arg ) = delete;
};

NAMESPACE_END( LOGLW )