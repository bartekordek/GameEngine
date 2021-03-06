#include "gameengine/Shader.hpp"
#include "gameengine/IUtility.hpp"
#include "gameengine/IGameEngine.hpp"

#include "CUL/CULInterface.hpp"
#include "CUL/Threading/ThreadUtils.hpp"

using namespace LOGLW;

Shader::Shader( IGameEngine& engine, CUL::FS::IFile* file ) : m_engine( engine ), m_shaderCode( file )
{
    create();
}

unsigned int Shader::getId() const
{
    return m_id;
}

void Shader::useShader() const
{

}

void Shader::reload()
{
    m_shaderCode->reload();
    release();
    create();
}

void Shader::create()
{
    auto createTask = [this]() {
        m_id = getUtility()->createShader( *m_shaderCode );
    };

    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo("RenderThread") )
    {
        createTask();
    }
    else
    {
        m_engine.addRenderThreadTask( [createTask]() {
            createTask();
        } );
    }
}

const CUL::FS::Path& Shader::getPath() const
{
    return m_shaderCode->getPath();
}

Shader::~Shader()
{
    release();
}

void Shader::release()
{
    if( m_id )
    {
        getUtility()->removeShader( m_id );
        m_id = 0;
    }
}