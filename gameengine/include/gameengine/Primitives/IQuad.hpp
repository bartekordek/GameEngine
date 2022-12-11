#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IRenderDevice.hpp"

#include "CUL/Graphics/SimpleSize3D.hpp"
#include "CUL/GenericUtils/DumbPtr.hpp"

NAMESPACE_BEGIN( LOGLW )

using Size = CUL::Graphics::SimpleSize3D<float>;

class IQuad: public IObject
{
public:
    GAME_ENGINE_API IQuad( IGameEngine* engine, bool forceLegacy );

    GAME_ENGINE_API virtual void setValues( const QuadData& values ) = 0;
    GAME_ENGINE_API virtual void setColor( const QuadColors& colors ) = 0;
    GAME_ENGINE_API virtual void setColor( const ColorS& color ) = 0;

    GAME_ENGINE_API virtual ~IQuad() = default;

protected:
private:
    IQuad() = delete;
    IQuad( const IQuad& arg ) = delete;
    IQuad( IQuad&& arg ) = delete;
    IQuad& operator=( const IQuad& rhv ) = delete;
    IQuad& operator=( IQuad&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )