#include "gameengine/IRenderable.hpp"
#include "gameengine/IGameEngine.hpp"
#include "DeviceOpenGL.hpp"

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

void IRenderable::loadShader( const CUL::FS::Path& shaderPath )
{
    auto device = static_cast<DeviceOpenGL*>( getEngine()->getDevice() );
    auto type = device->getShaderType( shaderPath.getExtension() );

}

IRenderable::~IRenderable()
{
    getEngine()->removeObjectToRender( this );
}