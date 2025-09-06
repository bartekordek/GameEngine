#pragma once

#include "gameengine/IEngineUser.hpp"

NAMESPACE_BEGIN( LOGLW )

class IObject;

class GAME_ENGINE_API IRenderable: public IEngineUser
{
public:
    IRenderable( bool renderOnMyOwn );
    IRenderable( const IRenderable& value ) = delete;
    IRenderable( IRenderable&& value ) = delete;
    IRenderable& operator=( const IRenderable& value ) = delete;
    IRenderable& operator=( IRenderable&& value ) = delete;

    void toggleRenderOnMyOwn( bool inEnable );

    virtual void render() = 0;

    IObject* getObject();
    void setObject( IObject* inObject );

    virtual ~IRenderable();

protected:
private:
    IObject* m_object{ nullptr };
    bool m_renderOnMyOwn{ true };

};

NAMESPACE_END( LOGLW )