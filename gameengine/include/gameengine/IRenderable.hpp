#pragma once

#include "gameengine/Import.hpp"

NAMESPACE_BEGIN( LOGLW )

class GAME_ENGINE_API IRenderable
{
public:
    IRenderable() = default;
    virtual ~IRenderable() = default;

    virtual void render() = 0;

protected:
private:
    IRenderable( const IRenderable& value ) = delete;
    IRenderable& operator=( const IRenderable& value ) = delete;
};

NAMESPACE_END( LOGLW )