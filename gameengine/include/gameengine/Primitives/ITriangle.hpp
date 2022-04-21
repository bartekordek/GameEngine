#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"

#include "CUL/GenericUtils/DumbPtr.hpp"
#include "CUL/Graphics/Pos3D.hpp"

NAMESPACE_BEGIN( LOGLW )

class GAME_ENGINE_API ITriangle:
    public IObject
{
public:
    ITriangle();

    virtual void setValues( const TriangleData& values ) = 0;
    virtual void setColor( const TriangleColors& colors ) = 0;
    virtual void setColor( const ColorS& color ) = 0;

    virtual ~ITriangle();
protected:
private:
    ITriangle( const ITriangle& args ) = delete;
    ITriangle( ITriangle&& args ) = delete;
    ITriangle& operator=( const ITriangle& rhv ) = delete;
    ITriangle& operator=( ITriangle&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )