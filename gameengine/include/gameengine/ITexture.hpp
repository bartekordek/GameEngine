#pragma once

#include "gameengine/Import.hpp"

#include "CUL/Graphics/SimpleSize2D.hpp"

NAMESPACE_BEGIN( LOGLW )

using TexSize = CUL::Graphics::SSize2Di;
using TexID = unsigned int;

class GAME_ENGINE_API ITexture
{
public:
    ITexture();

    virtual void render() = 0;

    virtual const TexSize& getSize() const = 0;

    virtual TexID getID() const = 0;

    virtual ~ITexture();
protected:
private:
    ITexture( const ITexture& arg ) = delete;
    ITexture( ITexture&& arg ) = delete;
    ITexture& operator=( const ITexture& rhv ) = delete;
    ITexture& operator=( ITexture&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )