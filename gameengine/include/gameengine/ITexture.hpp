#pragma once

#include "gameengine/Import.hpp"
#include "CUL/GenericUtils/NonCopyable.hpp"
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
    virtual void unbind() {};

    virtual ~ITexture();
protected:
private:
    CUL_NONCOPYABLE( ITexture )
};

NAMESPACE_END( LOGLW )