#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/Shaders/ShaderUnit.hpp"
#include "RunOnRenderThread.hpp"

#include "CUL/CULInterface.hpp"
#include "CUL/Threading/ThreadUtil.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"
#include "CUL/Filesystem/FileFactory.hpp"

#include "CUL/ObjectRegistry.hpp"

#include "CUL/STL_IMPORTS/STD_atomic.hpp"

using namespace LOGLW;

struct ShaderProgram::Shader
{

};

ShaderProgram::ShaderProgram() : m_engine( *IGameEngine::getInstance() )
{
    create();
    IName::AfterNameChangeCallback = [this]( const CUL::String& newName )
    {
        RunOnRenderThread::getInstance().Run(
            [this, newName]()
            {
                getDevice()->setObjectName( EObjectType::PROGRAM, m_id, newName );
            } );
    };
    setName( "ShaderProgram " + CUL::String( getId() ) );
}

void ShaderProgram::loadShader( const String& shaderPath )
{
    RunOnRenderThread::getInstance().Run(
        [this, shaderPath]()
        {
            ShaderUnit* su = getDevice()->createShaderUnit( shaderPath );
            std::unique_ptr<ShaderUnit> shaderPtr( su );
            m_shaders[su->Type] = std::move(shaderPtr);
            attachShader( su->ID );
            link();
            validate();
            su->State = EShaderUnitState::Loaded;
        } );
}

void ShaderProgram::attachShader( std::uint32_t id )
{
    getDevice()->attachShader( m_id, id );
}

void ShaderProgram::attachShaders()
{
    for( const auto& su : m_shaders )
    {
        getDevice()->attachShader( m_id, su.second->ID );
    }
}

void ShaderProgram::link()
{
    getDevice()->linkProgram(m_id);
}

void ShaderProgram::validate()
{
    getDevice()->validateProgram( m_id );
}

unsigned int ShaderProgram::getId() const
{
    return m_id;
}

void ShaderProgram::useShader() const
{

}

void ShaderProgram::reload()
{
    //m_shaderCode->reload();
    //release();
    //create();
}

void ShaderProgram::create()
{
    RunOnRenderThread::getInstance().Run(
        [this]()
        {
            m_id = getDevice()->createProgram( getName() );
        } );
}

CShaderTypes::ShaderType ShaderProgram::getType() const
{
    return m_type;
}

void ShaderProgram::setUniform( const String& name, const char* value )
{

}

void ShaderProgram::setUniform( const String& name, float value )
{
    const std::int32_t uniformId = getUniformLocation( name );
    getDevice()->setUniformValue( uniformId, value );
}

void ShaderProgram::setUniform( const String& name, unsigned value )
{
    const std::int32_t uniformId = getUniformLocation( name );
    getDevice()->setUniformValue( uniformId, value );
}

void ShaderProgram::setUniform( const String& name, int value )
{
    const std::int32_t uniformId = getUniformLocation( name );
    getDevice()->setUniformValue( uniformId, value );
}

void ShaderProgram::setUniform( const String& name, bool value )
{
    const std::int32_t uniformId = getUniformLocation( name );
    getDevice()->setUniformValue( uniformId, value );
}

void ShaderProgram::setUniform( const String& name, const glm::mat2& mat )
{
    const std::int32_t uniformId = getUniformLocation( name );
    getDevice()->setUniformValue( uniformId, mat );
}

void ShaderProgram::setUniform( const String& name, const glm::mat3& mat )
{
    const std::int32_t uniformId = getUniformLocation( name );
    getDevice()->setUniformValue( uniformId, mat );
}

void ShaderProgram::setUniform( const String& name, const glm::mat4& mat )
{
    const std::int32_t uniformId = getUniformLocation(name);
    getDevice()->setUniformValue(uniformId, mat);
}

void ShaderProgram::setUniform( const String& name, const glm::vec3& value )
{
    const std::int32_t uniformId = getUniformLocation( name );
    getDevice()->setUniformValue( uniformId, value );
}

void ShaderProgram::setUniform( const String& name, const glm::vec4& value )
{
    const std::int32_t uniformId = getUniformLocation( name );
    getDevice()->setUniformValue( uniformId, value );
}

String ShaderProgram::getAttributeStr( const String& name )
{
    const std::int32_t uniformId = getUniformLocation( name );
    return String();
}

float ShaderProgram::getAttributeF( const String& name )
{
    const std::int32_t uniformId = getUniformLocation( name );
    return std::get<float>( m_uniformMapping[name].Value );
}

unsigned int ShaderProgram::getAttributeUi( const String& name )
{
    const std::int32_t uniformId = getUniformLocation( name );
    return std::get<std::uint32_t>( m_uniformMapping[name].Value );
}

int ShaderProgram::getAttributeI( const String& name )
{
    const std::int32_t uniformId = getUniformLocation( name );
    return std::get<std::int32_t>( m_uniformMapping[name].Value );
}

void ShaderProgram::enable()
{
    getDevice()->useProgram( m_id );
}

void ShaderProgram::disable()
{
    getDevice()->useProgram( 0 );
}

EShaderUnitState ShaderProgram::getShaderUnitState( CShaderTypes::ShaderType inType ) const
{
    const auto it = m_shaders.find( inType );
    if( it == m_shaders.end() )
    {
        return EShaderUnitState::Unloaded;
    }

    return it->second->State;
}


ShaderProgram::~ShaderProgram()
{
    release();
}

void ShaderProgram::release()
{
    if( m_id )
    {
        auto removeShaderTask = [this]()
        {
            getDevice()->removeProgram( m_id );
        };

        if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
        {
            removeShaderTask();
        }
        else
        {
            std::atomic_bool shaderRemoved = false;
            m_engine.addPreRenderTask(
                [removeShaderTask, &shaderRemoved]()
                {
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

std::int32_t ShaderProgram::getUniformLocation( const String& name ) const
{
    getDevice()->useProgram( m_id );

    const auto it = m_uniformMapping.find( name );
    if( it == m_uniformMapping.end() )
    {
        const std::int32_t id = getDevice()->getUniformLocation( m_id, name );
        ShaderVariable sv;
        sv.Id = id;
        sv.Name = name;
        m_uniformMapping[name] = sv;
        return id;
    }

    return it->second.Id;
}
