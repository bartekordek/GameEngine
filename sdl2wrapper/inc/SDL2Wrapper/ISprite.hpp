#pragma once

#include "SDL2Wrapper/Import.hpp"

#include "CUL/Graphics/IObject.hpp"

NAMESPACE_BEGIN( CUL::Graphics )
class ITexture;
NAMESPACE_END( CUL::Graphics )

NAMESPACE_BEGIN( SDL2W )

class SDL2WAPI ISprite: public CUL::Graphics::IObject
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

NAMESPACE_END( SDL2W )