#pragma once

#include "gameengine/IEngineUser.hpp"
#include "CUL/GenericUtils/NonCopyable.hpp"

NAMESPACE_BEGIN( LOGLW )

class IObject;

class GAME_ENGINE_API IRenderable: public IEngineUser
{
public:
    IRenderable( bool renderOnMyOwn );

    void toggleRenderOnMyOwn( bool inEnable );

    virtual void render() = 0;

    IObject* getObject();
    void setObject( IObject* inObject );

    virtual ~IRenderable();

    CUL_NONCOPYABLE( IRenderable )
protected:
private:
    IObject* m_object{ nullptr };
    bool m_renderOnMyOwn{ true };

};

NAMESPACE_END( LOGLW )