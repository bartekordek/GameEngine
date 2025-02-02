#pragma once

#include "gameengine/Import.hpp"

#include "CUL/Graphics/IObject.hpp"

NAMESPACE_BEGIN( CUL::Graphics )
class ITexture;
NAMESPACE_END( CUL::Graphics )

NAMESPACE_BEGIN( LOGLW )

class GAME_ENGINE_API ISprite: public CUL::Graphics::IObject
{
public:
    ISprite();
    ISprite( const ISprite& ) = delete;
    ISprite( ISprite&& ) = delete;
    ISprite& operator=( const ISprite& ) = delete;
    ISprite& operator=( ISprite&& ) = delete;

    virtual CUL::Graphics::ITexture* getTexture() = 0;
    virtual void setTexture( CUL::Graphics::ITexture* texture ) = 0;

    virtual ~ISprite();

private:
};

NAMESPACE_END( LOGLW )