#include "gameengine/Shader.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/IGameEngine.hpp"

#include "CUL/CULInterface.hpp"
#include "CUL/Threading/ThreadUtils.hpp"

#include "CUL/STL_IMPORTS/STD_atomic.hpp"

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
        m_id = getDevice()->createShader( *m_shaderCode );
    };

    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo("RenderThread") )
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

void Shader::addUsedFrom( Program* inProgram )
{
    m_usedFromList.push_back( inProgram );
}

void Shader::removeUsedFrom( Program* inProgram )
{
    auto it = std::find( m_usedFromList.begin(), m_usedFromList.end(), inProgram );
    if( it != m_usedFromList.end() )
    {
        m_usedFromList.erase( it );
    }
}

size_t Shader::getUsedFromCount() const
{
    return m_usedFromList.size();
}

Shader::~Shader()
{
    release();
}

void Shader::release()
{
    if( m_id )
    {
        auto removeShaderTask = [this]() {
            getDevice()->removeShader( m_id );
        };

        if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
        {
            removeShaderTask();
        }
        else
        {
            std::atomic_bool shaderRemoved = false;
            m_engine.addRenderThreadTask( [removeShaderTask, &shaderRemoved]() {
                removeShaderTask();
                shaderRemoved = true;
            } );

            while( !shaderRemoved )
            {
            }
        }

        m_id = 0;
    }
}