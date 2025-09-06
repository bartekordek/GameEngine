#include "gameengine/IRenderable.hpp"
#include "gameengine/IGameEngine.hpp"

using namespace LOGLW;

IRenderable::IRenderable( bool renderOnMyOwn ):
    IEngineUser(),
    m_renderOnMyOwn( renderOnMyOwn )
{
    toggleRenderOnMyOwn( renderOnMyOwn );
}

void IRenderable::toggleRenderOnMyOwn( bool inEnable )
{
    if( inEnable )
    {
        getEngine().addObjectToRender( this );
        m_renderOnMyOwn = true;
    }
    else if( m_renderOnMyOwn )
    {
        getEngine().removeObjectToRender( this );
        m_renderOnMyOwn = false;
    }
}

IObject* IRenderable::getObject()
{
    return m_object;
}

void IRenderable::setObject( IObject* inObject )
{
    m_object = inObject;
}

IRenderable::~IRenderable()
{
    if( m_renderOnMyOwn )
    {
        getEngine().removeObjectToRender( this );
    }
}