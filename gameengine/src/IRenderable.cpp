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
    if( m_program )
    {
        delete m_program;
        m_program = nullptr;
    }



    auto device = static_cast<DeviceOpenGL*>( getEngine()->getDevice() );
    ShaderTypes type = device->getShaderType( shaderPath.getExtension() );
    if( type == ShaderTypes::FRAGMENT_SHADER )
    {

    }
}

IRenderable::~IRenderable()
{
    getEngine()->removeObjectToRender( this );
}