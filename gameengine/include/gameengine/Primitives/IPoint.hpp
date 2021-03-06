#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtility.hpp"

NAMESPACE_BEGIN( LOGLW )

class GAME_ENGINE_API IPoint:
    public IObject
{
public:
    IPoint( IGameEngine* engine ) : IObject( engine )
    {
    }

    virtual void setColor( const ColorS& color ) = 0;

    virtual ~IPoint() = default;

protected:
private:
    IPoint( const IPoint& arg ) = delete;
    IPoint( IPoint&& arg ) = delete;
    IPoint& operator=( const IPoint& rhv ) = delete;
    IPoint& operator=( IPoint&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )