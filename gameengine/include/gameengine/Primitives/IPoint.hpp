#pragma once

#include "gameengine/IObject.hpp"

NAMESPACE_BEGIN( LOGLW )

class IPoint:
    public IObject
{
public:
    GAME_ENGINE_API IPoint( IGameEngine* engine, bool forceLegacy ): IObject( engine, forceLegacy )
    {
    }

    GAME_ENGINE_API virtual void setColor( const ColorS& color ) = 0;

    GAME_ENGINE_API virtual ~IPoint() = default;

protected:
private:
    IPoint( const IPoint& arg ) = delete;
    IPoint( IPoint&& arg ) = delete;
    IPoint& operator=( const IPoint& rhv ) = delete;
    IPoint& operator=( IPoint&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )