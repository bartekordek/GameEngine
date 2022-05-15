#pragma once

#include "gameengine/IEngineUser.hpp"

NAMESPACE_BEGIN( LOGLW )

class GAME_ENGINE_API IRenderable: public IEngineUser
{
public:
    IRenderable( IGameEngine* engine );
    virtual ~IRenderable() = default;

    void setDisableRenderOnMyOwn( bool disable );

    virtual void render() = 0;

protected:
private:
    IRenderable( const IRenderable& value ) = delete;
    IRenderable& operator=( const IRenderable& value ) = delete;
};

NAMESPACE_END( LOGLW )