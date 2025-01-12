#include "gameengine/IRenderable.hpp"
#include "gameengine/IGameEngine.hpp"

using namespace LOGLW;

IRenderable::IRenderable( IGameEngine* engine ) : IEngineUser( engine )
{
    getEngine()->addObjectToRender( this );
}

void IRenderable::setDisableRenderOnMyOwn( bool disable )
{
    if( disable )
    {
        getEngine()->removeObjectToRender( this );
    }
    else
    {
        getEngine()->addObjectToRender( this );
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
    getEngine()->removeObjectToRender( this );
}