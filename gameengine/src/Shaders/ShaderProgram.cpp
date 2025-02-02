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


ShaderProgram::ShaderProgram() : m_engine( *IGameEngine::getInstance() )
{
    create();
    IName::AfterNameChangeCallback = [this]( const CUL::String& newName )
    {
        RunOnRenderThread::getInstance().Run(
            [this, newName]()
            {
                getDevice()->setObjectName( EObjectType::PROGRAM, m_shaderProgramId, newName );
            } );
    };
    setName( "ShaderProgram " + CUL::String( getId() ) );
}

void ShaderProgram::compileLinkValidate()
{
    RunOnRenderThread::getInstance().Run(
        [this]()
        {
            //TODO
        } );
}

void ShaderProgram::reCompileWholeShader()
{
    std::vector<CUL::String> shadersPaths( m_shaders.size() );

    std::size_t shaderIndex{ 0 };
    for( auto& [shaderType, shaderUnit] : m_shaders )
    {
        shadersPaths[shaderIndex++] = shaderUnit->File->getPath().getPath();
        detachShader( shaderUnit->ID );
        getDevice()->deleteShaderUnit( shaderUnit );
    }

    for( const CUL::String& path : shadersPaths )
    {
        compileShader( path );
    }
    link();
}

void ShaderProgram::reCompileShader( const String& shaderPathString )
{
    CUL::String errorContent;
    reCompileShader( shaderPathString, true, errorContent );
}

void ShaderProgram::reCompileShader( const String& shaderPathString, bool assertOnErrors, CUL::String& errorMessage )
{
    const CUL::FS::Path shaderPath( shaderPathString );
    const auto extension = shaderPath.getExtension();
    CShaderTypes::ShaderType type = CShaderTypes::getShaderType( extension );
    const auto it = m_shaders.find( type );
    if( it != m_shaders.end() )
    {
        detachShader( it->second->ID );
        getDevice()->deleteShaderUnit( it->second );
        m_shaders.erase( it );
    }
    compileShader( shaderPathString, assertOnErrors, errorMessage );
}

void ShaderProgram::compileShader( const String& shaderPath )
{
    CUL::String errorContent;
    compileShader( shaderPath, true, errorContent );
}

void ShaderProgram::compileShader( const String& shaderPath, bool assertOnErrors, CUL::String& errorMessage )
{
    ShaderUnit* su = getDevice()->createShaderUnit( shaderPath, assertOnErrors, errorMessage );
    if( errorMessage.empty() )
    {
        m_shaders[su->Type] = su;
        attachShader( su->ID );
        su->State = EShaderUnitState::Loaded;
    }
    else
    {
        su->State = EShaderUnitState::Error;
    }
}


void ShaderProgram::attachShader( std::uint32_t id )
{
    getDevice()->attachShader( m_shaderProgramId, id );
}

void ShaderProgram::attachShaders()
{
    for( const auto& su : m_shaders )
    {
        getDevice()->attachShader( m_shaderProgramId, su.second->ID );
    }
}

void ShaderProgram::detachShader( std::uint32_t id )
{
    getDevice()->dettachShader( m_shaderProgramId, id );
}

void ShaderProgram::link()
{
    RunOnRenderThread::getInstance().Run(
        [this]()
        {
            linkImpl();
        } );
}

void ShaderProgram::linkImpl()
{
    getDevice()->linkProgram( m_shaderProgramId );
    m_linked = true;
    m_uniformMapping.clear();  // Might have changed on link.

    const auto attributes = getDevice()->fetchProgramAttributeInfo( m_shaderProgramId );
    for( const auto& attribute : attributes )
    {
        ShaderVariable sv;
        sv.Name = attribute.Name;
        sv.Id = attribute.ID;
        sv.Size = attribute.Size;
        sv.TypeName = attribute.TypeName;
        sv.Type = attribute.Type;
        m_attributeMapping[attribute.Name] = sv;
    }

    const auto uniforms = getDevice()->fetchProgramUniformsInfo( m_shaderProgramId );
    for( const auto& uniform : uniforms )
    {
        ShaderVariable sv;
        sv.Name = uniform.Name;
        sv.Id = uniform.ID;
        sv.Size = uniform.Size;
        sv.TypeName = uniform.TypeName;
        sv.Type = uniform.Type;
        m_uniformMapping[uniform.Name] = sv;
    }
}

void ShaderProgram::validate()
{
    getDevice()->validateProgram( m_shaderProgramId );
}

unsigned int ShaderProgram::getId() const
{
    return m_shaderProgramId;
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
            m_shaderProgramId = getDevice()->createProgram( getName() );
        } );
}

CShaderTypes::ShaderType ShaderProgram::getType() const
{
    return m_type;
}


void ShaderProgram::setUniform( const String& inName, UniformValue inValue )
{
    RunOnRenderThread::getInstance().Run(
        [this, inName, inValue]()
        {
            setUniformImpl( inName, inValue );
        } );
}

void ShaderProgram::setUniformImpl( const String& inName, UniformValue inValue )
{
    const auto it = m_uniformMapping.find( inName );
    if( it == m_uniformMapping.end() )
    {
        return;
    }

    ShaderVariable& sv = it->second;

    enable();
    switch( sv.Type )
    {
        case DataType::FLOAT:
        {
            getDevice()->setUniformValue( sv.Id, std::get<float>( inValue ) );
            break;
        }
        case DataType::FLOAT_MAT4:
        {
            getDevice()->setUniformValue( sv.Id, std::get<glm::mat4>( inValue ) );
            break;
        }
        case DataType::FLOAT_VEC3:
        {
            getDevice()->setUniformValue( sv.Id, std::get<glm::vec3>( inValue ) );
            break;
        }
        case DataType::FLOAT_VEC4:
        {
            getDevice()->setUniformValue( sv.Id, std::get<glm::vec4>( inValue ) );
            break;
        }
        case DataType::SAMPLER_1D:
        {
            getDevice()->setUniformValue( sv.Id, std::get<std::int32_t>( inValue ) );
            break;
        }
        case DataType::SAMPLER_2D:
        {
            getDevice()->setUniformValue( sv.Id, std::get<std::int32_t>( inValue ) );
            break;
        }
        default:
            throw std::logic_error( "Method not implemented" );
    }

    sv.Value = inValue;
    sv.Applied = true;
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
    if( m_linked )
    {
        getDevice()->useProgram( m_shaderProgramId );
    }
}

void ShaderProgram::disable()
{
    if( m_linked )
    {
        getDevice()->useProgram( 0 );
    }
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

const ShaderProgram::ShaderVariable& ShaderProgram::getUniformValue( const CUL::String& name )
{
    const auto it = m_uniformMapping.find( name );
    ShaderVariable& result = it->second;

    if( result.Applied )
    {
        return result;
    }
    LOGLW::UniformValue uv = getDevice()->getUniformValue( m_shaderProgramId, result.Id, result.Type );
    result.Applied = true;
    result.Value = uv;

    return result;
}

std::vector<CUL::String> ShaderProgram::getUniformsNames()
{
    std::vector<CUL::String> result;

    for( const auto& uniform : m_uniformMapping )
    {
        result.push_back( uniform.first );
    }

    return result;
}

ShaderProgram::~ShaderProgram()
{
    release();
}

void ShaderProgram::release()
{
    if( m_shaderProgramId )
    {
        auto removeShaderTask = [this]()
        {
            getDevice()->removeProgram( m_shaderProgramId );
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

        m_shaderProgramId = 0;
    }
    m_shaders.clear();
    m_uniformMapping.clear();
}

bool ShaderProgram::getIsLinked() const
{
    return m_linked;
}

std::int32_t ShaderProgram::getUniformLocation( const String& name ) const
{
    getDevice()->useProgram( m_shaderProgramId );

    const auto it = m_uniformMapping.find( name );
    if( it == m_uniformMapping.end() )

    {
        return -1;
    }

    return it->second.Id;
}
