#include "gameengine/Program.hpp"
#include "gameengine/IGameEngine.hpp"

#include "CUL/CULInterface.hpp"
#include "CUL/Threading/ThreadUtils.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"
#include "CUL/Filesystem/FileFactory.hpp"

using namespace LOGLW;

Program::Program( IGameEngine& engine ) : m_engine( engine )
{
    m_logger = m_engine.getLoger();

    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        initialize();
    }
    else
    {
        m_engine.addRenderThreadTask( [this]() {
            initialize();
        } );
    }
}

bool Program::initialized() const
{
    return m_id != 0;
}

void Program::initialize()
{
    m_id = getUtility()->createProgram();
}


void Program::setAttrib( const String&, const char* )
{
    // TODO
}

void Program::setAttrib( const String& name, float value )
{
    ValueToSet task;
    task.name = name;
    task.value = value;
    pushTask( task );
}

void Program::setAttrib( const String& name, unsigned value )
{
    auto location = getUtility()->getUniformLocation( m_id, name );
    getUtility()->setAttribValue( location, value );
}

void Program::setAttrib( const String& name, int value )
{
    auto location = getUtility()->getUniformLocation( m_id, name );
    getUtility()->setAttribValue( location, value );
}

void Program::setAttrib( const String& name, bool value )
{
    auto location = getUtility()->getUniformLocation( m_id, name );
    getUtility()->setAttribValue( location, value );
}

void Program::setAttrib( const String& name, const glm::mat2& value )
{
    auto location = getUtility()->getUniformLocation( m_id, name );
    getUtility()->setUniformValue( location, value );
}

void Program::setAttrib( const String& name, const glm::mat3& value )
{
    auto location = getUtility()->getUniformLocation( m_id, name );
    getUtility()->setUniformValue( location, value );
}

void Program::setAttrib( const String& name, const glm::mat4& val )
{
    auto location = getUtility()->getUniformLocation( m_id, name );
    //const std::string valueAsString = glm::to_string( val );
    //getUtility()->getCUl()->getLogger()->log( "setAttrib: " + name + ", val: " + valueAsString );
    getUtility()->setUniformValue( location, val );
}

Shader* Program::loadShader( const char* path )
{
    auto fragmentShaderFile = getUtility()->getCUl()->getFF()->createRegularFileRawPtr( path );
    fragmentShaderFile->load( true );
    Shader* result = new Shader( m_engine, fragmentShaderFile );
    attachShader( result );
    link();
    validate();
    return result;
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
    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        reloadShaderImpl();
    }
    else
    {
        m_engine.addRenderThreadTask(
            [this]()
            {
                reloadShaderImpl();
            } );
    }
}

void Program::reloadShaderImpl()
{
    std::vector<String> shadersPaths;
    for( const auto shader : m_attachedShaders )
    {
        shadersPaths.push_back(shader.first);
    }

    release();
    initialize();

    for( auto shaderPath : shadersPaths )
    {
        auto shaderFile = getUtility()->getCUl()->getFF()->createFileFromPath( shaderPath );
        shaderFile->load(true);
        auto shader = new Shader( m_engine, shaderFile );
        attachShader( shader );
    }

    link();
    validate();
}

void Program::attachShader( Shader* shader )
{
    std::lock_guard<std::mutex> lock( m_operationMutex );
    auto shaderId = shader->getId();

    auto attachTask = [this, shaderId]() {
        getUtility()->attachShader( m_id, shaderId );
    };

    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        attachTask();
    }
    else
    {
        m_engine.addRenderThreadTask( [attachTask]() {
            attachTask();
        } );
    }


    m_attachedShaders[shader->getPath()] = shader;
}

void Program::dettachShader( Shader* shader )
{
    std::lock_guard<std::mutex> lock( m_operationMutex );
    auto it = m_attachedShaders.find( shader->getPath() );
    m_attachedShaders.erase( it );
}

void Program::link()
{
    auto linkTask = [this]() {
        std::lock_guard<std::mutex> lock( m_operationMutex );
        getUtility()->linkProgram( m_id );
    };

    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        linkTask();
    }
    else
    {
        m_engine.addRenderThreadTask( [linkTask]() {
            linkTask();
        } );
    }
}

void Program::enable()
{
    std::lock_guard<std::mutex> lock( m_operationMutex );
    getUtility()->useProgram( m_id );
}

void Program::disable()
{
    getUtility()->useProgram( 0 );
}

void Program::validate()
{
    auto validateTask = [this]() {
        std::lock_guard<std::mutex> lock( m_operationMutex );
        getUtility()->validateProgram( m_id );
    };

    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        validateTask();
    }
    else
    {
        m_engine.addRenderThreadTask( [validateTask]() {
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
    std::lock_guard<std::mutex> tasksGuard( m_operationMutex );
    while( !m_tasks.empty() )
    {
        processTask( m_tasks.front() );
        m_tasks.pop_front();
    }
}

void Program::pushTask( ValueToSet& task )
{
    std::lock_guard<std::mutex> tasksGuard( m_operationMutex );
    m_tasks.push_back( task );
}

void Program::processTask( const ValueToSet& task )
{
    {
        auto floatValue = std::get_if<float>( &task.value );
        if( floatValue )
        {
            auto location = getUtility()->getUniformLocation( m_id, task.name );
            getUtility()->setAttribValue( location, *floatValue );
            return;
        }
    }

    {
        auto unsignedValue = std::get_if<unsigned>( &task.value );
        if( unsignedValue )
        {
            auto location = getUtility()->getUniformLocation( m_id, task.name );
            getUtility()->setAttribValue( location, *unsignedValue );
            return;
        }
    }

    {
        auto intValue = std::get_if<unsigned>( &task.value );
        if( intValue )
        {
            auto location = getUtility()->getUniformLocation( m_id, task.name );
            getUtility()->setAttribValue( location, *intValue );
        }
    }
}

const ShaderList& Program::getShaderList() const
{
    return m_attachedShaders;
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
    const auto shadersSize = m_attachedShaders.size();
    String logVal;
    logVal = String( "Shaders to free: " ) + String( (int)shadersSize );
    m_logger->log( logVal );

    for( auto shaderPair : m_attachedShaders )
    {
        delete shaderPair.second;
    }

    m_attachedShaders.clear();

    releaseProgram();
}

void Program::releaseProgram()
{
    getUtility()->removeProgram( m_id );
    m_id = 0;
}