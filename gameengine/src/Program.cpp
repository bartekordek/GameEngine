#include "gameengine/Program.hpp"
#include "gameengine/IGameEngine.hpp"
#include "RunOnRenderThread.hpp"

#include "CUL/CULInterface.hpp"
#include "CUL/Threading/ThreadUtil.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"
#include "CUL/Filesystem/FileFactory.hpp"

#include "CUL/STL_IMPORTS/STD_atomic.hpp"

using namespace LOGLW;

Program::Program( IGameEngine& engine ) : m_engine( engine )
{
    m_logger = m_engine.getLoger();
    RunOnRenderThread::getInstance().Run(
        [this]()
        {
            initialize();
        } );

    IName::AfterNameChangeCallback = [this]( const CUL::String& newName )
    {
        RunOnRenderThread::getInstance().Run(
            [this, newName]()
            {
                getDevice()->setObjectName( EObjectType::PROGRAM, m_id, newName );
            } );
    };

    setName( "program_" + CUL::String( getId() ) );
}

bool Program::initialized() const
{
    return m_id != 0;
}

void Program::initialize()
{
    if( getName().empty() )
    {
        setName( "Shader Program" );
    }

    m_id = getDevice()->createProgram( getName() );
    m_initialized = true;
}

void Program::setUniform( const String&, const char* )
{
    // TODO
}

void Program::setUniform( const String& name, float value )
{
    ValueToSet task;
    task.name = name;
    task.value = value;
    pushTask( task );
}

void Program::setUniform( const String& name, unsigned value )
{
    getDevice()->useProgram( static_cast<int>( m_id ) );

    auto location = getUniformLocation( name );
    getDevice()->setUniformValue( location, value );
}

void Program::setUniform( const String& name, int value )
{
    getDevice()->useProgram( m_id );

    auto location = getUniformLocation( name );
    getDevice()->setUniformValue( location, value );
}

void Program::setUniform( const String& name, bool value )
{
    getDevice()->useProgram( m_id );

    auto location = getUniformLocation( name );
    getDevice()->setUniformValue( location, value );
}

void Program::setUniform( const String& name, const glm::mat2& value )
{
    getDevice()->useProgram( m_id );

    auto location = getUniformLocation( name );
    getDevice()->setUniformValue( location, value );
}

void Program::setUniform( const String& name, const glm::mat3& value )
{
    auto location = getUniformLocation( name );
    getDevice()->setUniformValue( location, value );
}

void Program::setUniform( const String& name, const glm::mat4& val )
{
    getDevice()->useProgram( m_id );

    auto location = getUniformLocation( name );
    getDevice()->setUniformValue( location, val );
}

void Program::setUniform( const String& name, const glm::vec3& value )
{
    getDevice()->useProgram( m_id );
    auto location = getUniformLocation( name );
    getDevice()->setUniformValue( location, value );
}

void Program::setUniform( const String& name, const glm::vec4& value )
{
    getDevice()->useProgram( m_id );
    auto location = getUniformLocation( name );
    if( location != -1 )
    {
        getDevice()->setUniformValue( location, value );
    }
}

Shader* Program::loadShader( const char* path )
{
    auto fragmentShaderFile = CUL::CULInterface::getInstance()->getFF()->createRegularFileRawPtr( path );
    fragmentShaderFile->load( true );
    Shader* result = new Shader( m_engine, fragmentShaderFile );
    attachShader( result );
    link();
    validate();

    m_uniformMap.clear();


    return result;
}

int Program::getUniformLocation( const String& name )
{
    auto it = m_uniformMap.find( name );
    if( it == m_uniformMap.end() )
    {
        auto location = getDevice()->getUniformLocation( m_id, name );
        if( location == -1 )
        {
            return location;
        }

        m_uniformMap[name] = location;
        return location;
    }
    return it->second;
}

String Program::getAttributeStr( const String& )
{
    return String();
}

float Program::getAttributeF( const String& )
{
    return float();
}

unsigned int Program::getAttributeUi( const String& )
{
    return unsigned();
}

int Program::getAttributeI( const String& )
{
    return int();
}

void Program::reloadShader()
{
    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        reloadShaderImpl();
    }
    else
    {
        m_engine.addPreRenderTask(
            [this]()
            {
                reloadShaderImpl();
            } );
    }
}

void Program::reloadShaderImpl()
{
    std::vector<String> shadersPaths;
    for( const auto [_, shader] : m_attachedShaders )
    {
        shadersPaths.push_back( shader->getPath() );
        dettachShader( shader );
        m_uniformMap.clear();
    }

    for( auto shaderPath : shadersPaths )
    {
        auto shaderFile = CUL::CULInterface::getInstance()->getFF()->createFileFromPath( shaderPath );
        shaderFile->load(true);
        auto shader = new Shader( m_engine, shaderFile );
        attachShader( shader );
    }

    link();
    validate();
}

void Program::attachShader( Shader* shader )
{
    auto it = m_attachedShaders.find( shader->getType() );
    Shader* oldSHader = nullptr; 
    if( it != m_attachedShaders.end() )
    {
        oldSHader = it->second;
    }

    if( oldSHader )
    {
        dettachShader( oldSHader );
    }

    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        attachShaderImpl( shader );
    }
    else
    {
        m_engine.addPreRenderTask( [this, shader]() {
            attachShaderImpl( shader );
        } );
    }
}

void Program::attachShaderImpl( Shader* shader )
{
    auto shaderId = shader->getId();
    m_attachedShaders[shader->getType()] = shader;
    shader->addUsedFrom( this );
    getDevice()->attachShader( m_id, shaderId );
}

void Program::dettachShader( Shader* shader )
{
    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        dettachShaderImpl( shader );
    }
    else
    {
        m_engine.addPreRenderTask( [this, shader]() {
            dettachShaderImpl( shader );
        } );
    }
}

void Program::dettachShaderImpl( Shader* shader )
{
    m_attachedShaders[shader->getType()] = nullptr;
    shader->removeUsedFrom( this );
    getDevice()->dettachShader( m_id, shader->getId() );
}

void Program::link()
{
    auto linkTask = [this]() {
        getDevice()->linkProgram( m_id );
    };

    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        linkTask();
    }
    else
    {
        m_engine.addPreRenderTask( [linkTask]() {
            linkTask();
        } );
    }
}

void Program::enable()
{
    getDevice()->useProgram( m_id );
}

void Program::disable()
{
    getDevice()->useProgram( 0 );
}

void Program::validate()
{
    auto validateTask = [this]() {
        getDevice()->validateProgram( m_id );
    };

    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        validateTask();
    }
    else
    {
        m_engine.addPreRenderTask( [validateTask]() {
            validateTask();
        } );
    }
}

void Program::render()
{
    enable();
    goThroughTasks();
}

void Program::goThroughTasks()
{
    while( !m_preRenderTasks.empty() )
    {
        processTask( m_preRenderTasks.front() );
        m_preRenderTasks.pop_front();
    }
}

void Program::pushTask( ValueToSet& task )
{
    m_preRenderTasks.push_back( task );
}

void Program::processTask( const ValueToSet& task )
{
    {
        auto floatValue = std::get_if<float>( &task.value );
        if( floatValue )
        {
            auto location = getDevice()->getUniformLocation( m_id, task.name );
            getDevice()->setAttribValue( location, *floatValue );
            return;
        }
    }

    {
        auto unsignedValue = std::get_if<unsigned>( &task.value );
        if( unsignedValue )
        {
            auto location = getDevice()->getUniformLocation( m_id, task.name );
            getDevice()->setAttribValue( location, *unsignedValue );
            return;
        }
    }

    {
        auto intValue = std::get_if<unsigned>( &task.value );
        if( intValue )
        {
            auto location = getDevice()->getUniformLocation( m_id, task.name );
            getDevice()->setAttribValue( location, *intValue );
        }
    }
}

// TODO
void Program::bufferData( std::vector<float>&, const BufferTypes )
{
}

unsigned int Program::getProgramId()
{
    return m_id;
}

Program::~Program()
{
    release();
}

void Program::release()
{
    auto releaseTask = [this]() {
        const auto shadersSize = m_attachedShaders.size();
        String logVal;
        logVal = String( "Shaders to free: " ) + String( (int)shadersSize );
        m_logger->log( logVal );

        for( auto shaderPair : m_attachedShaders )
        {
            Shader* shader = shaderPair.second;
            shader->removeUsedFrom( this );
            getDevice()->dettachShader( m_id, shader->getId() );
            if( shader->getUsedFromCount() == 0 )
            {
                m_engine.removeShader( shader );
            }
        }

        m_attachedShaders.clear();

        releaseProgram();
    };

    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        releaseTask();
    }
    else
    {
        std::atomic_bool released = false;
        m_engine.addPreRenderTask( [releaseTask, &released]() {
            releaseTask();
            released = true;
        } );

        while( !released )
        {
        }
    }
}

void Program::releaseProgram()
{
    String logValue = "Releasing program: "  + String( m_id ) + "...";
    m_logger->log( logValue );
    auto removeShaderTask = [this]() {
        getDevice()->removeProgram( m_id );
        m_id = 0;
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

    m_logger->log( logValue + " done." );
}