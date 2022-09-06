#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"

#include "CUL/GenericUtils/DumbPtr.hpp"
#include "CUL/Graphics/Pos3D.hpp"

NAMESPACE_BEGIN( LOGLW )

class ITriangle:
    public IObject
{
public:
    GAME_ENGINE_API ITriangle( IGameEngine* engine );

    GAME_ENGINE_API virtual void setValues( const TriangleData& values ) = 0;
    GAME_ENGINE_API virtual void setColor( const TriangleColors& colors ) = 0;
    GAME_ENGINE_API virtual void setColor( const ColorS& color ) = 0;

    GAME_ENGINE_API virtual ~ITriangle();

protected:
private:
    ITriangle( const ITriangle& args ) = delete;
    ITriangle( ITriangle&& args ) = delete;
    ITriangle& operator=( const ITriangle& rhv ) = delete;
    ITriangle& operator=( ITriangle&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )