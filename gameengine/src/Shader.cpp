#include "gameengine/Shader.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/IGameEngine.hpp"
#include "RunOnRenderThread.hpp"

#include "CUL/CULInterface.hpp"
#include "CUL/Threading/ThreadUtil.hpp"
#include "CUL/ObjectRegistry.hpp"

#include "CUL/STL_IMPORTS/STD_atomic.hpp"

using namespace LOGLW;

Shader::Shader( IGameEngine& engine, CUL::FS::IFile* file ) : m_engine( engine ), m_shaderCode( file )
{
    create();

    IName::AfterNameChangeCallback = [this]( const CUL::String& newName )
    {
        RunOnRenderThread::getInstance().Run(
            [this, newName]()
            {
                getDevice()->setObjectName( EObjectType::SHADER, m_id, newName );
            } );
    };
    setName( "Shader " + CUL::String( getId() ) );
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
        m_type = CShaderTypes::getShaderType( m_shaderCode->getPath().getExtension() );
    };

    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo("RenderThread") )
    {
        createTask();
    }
    else
    {
        m_engine.addPreRenderTask( [createTask]() {
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

CShaderTypes::ShaderType Shader::getType() const
{
    return m_type;
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
            m_engine.addPreRenderTask( [removeShaderTask, &shaderRemoved]() {
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