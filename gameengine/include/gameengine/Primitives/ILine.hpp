#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IRenderDevice.hpp"

NAMESPACE_BEGIN( LOGLW )

class ILine: public IObject
{
public:
    GAME_ENGINE_API ILine( IGameEngine* engine, bool forceLegacy ) : IObject( "Line", engine, forceLegacy )
    {
    }

    GAME_ENGINE_API virtual void setValues( const LineData& values ) = 0;
    GAME_ENGINE_API virtual void setColor( const LineColors& colors ) = 0;
    GAME_ENGINE_API virtual void setColor( const ColorS& color ) = 0;

    GAME_ENGINE_API virtual ~ILine() = default;

protected:
private:
    ILine( const ILine& arg ) = delete;
    ILine( ILine&& arg ) = delete;
    ILine& operator=( const ILine& rhv ) = delete;
    ILine& operator=( ILine&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )