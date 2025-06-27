#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/ExecuteType.hpp"
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
#include "CUL/IMPORT_tracy.hpp"

using namespace LOGLW;

ShaderProgram::ShaderProgram()
    : m_engine( *IGameEngine::getInstance() )
{
    create();
}

void ShaderProgram::createFrom( EExecuteType inEt, const ShadersData& inShaderData )
{
    if( inEt == EExecuteType::Now )
    {
        createFromImpl( EExecuteType::Now, inShaderData );
    }
    else if( inEt == EExecuteType::WaitForCompletion )
    {
        RunOnRenderThread::getInstance().RunWaitForResult(
            [this, &inShaderData]()
            {
                createFromImpl( EExecuteType::Now, inShaderData );
            } );
    }
    else if( inEt == EExecuteType::ExecuteOnRenderThread )
    {
        m_tasks.addTask(
            [this, inShaderData]()
            {
                createFromImpl( EExecuteType::Now, inShaderData );
            } );
    }
}

void ShaderProgram::createFromImpl( EExecuteType inEt, const ShadersData& inShaderData )
{
    releaseShaderUnits();

    compileShader( inEt, inShaderData.FragmentShader );
    compileShader( inEt, inShaderData.VertexShader );
    link( inEt );
    validate();
}

void ShaderProgram::onNameChange( const String& newName )
{
    CUL::IName::onNameChange( newName );
    m_tasks.addTask(
        [this, newName]()
        {
            getDevice()->setObjectName( EObjectType::PROGRAM, m_shaderProgramId, newName );
        } );
}

void ShaderProgram::compileLinkValidate( EExecuteType /*inEt*/ )
{
    //TODO:
    //RunOnRenderThread::getInstance().Run(
    //    [this]()
    //    {
    //        //TODO
    //    } );
}

void ShaderProgram::reCompileWholeShader( EExecuteType inEt )
{
    if( inEt == EExecuteType::WaitForCompletion )
    {
        RunOnRenderThread::getInstance().Run(
            [this, inEt]()
            {
                reCompileWholeShaderImpl( inEt );
            } );
    }
    else if( inEt == EExecuteType::None )
    {
        reCompileWholeShaderImpl( inEt );
    }
}

void ShaderProgram::reCompileWholeShaderImpl( EExecuteType inEt )
{
    std::vector<CUL::String> shadersPaths( m_shaders.size() );
    std::size_t shaderIndex{ 0 };
    for( const auto& [shaderType, shaderUnit] : m_shaders )
    {
        shadersPaths[shaderIndex++] = shaderUnit->File->getPath().getPath();
    }

    releaseShaderUnits();

    for( const CUL::String& path : shadersPaths )
    {
        compileShader( inEt, path );
    }
    link( inEt );
}

void ShaderProgram::releaseShaderUnits()
{
    for( auto& [shaderType, shaderUnit] : m_shaders )
    {
        detachShader( EExecuteType::Now, shaderUnit->ID );
        getDevice()->deleteShaderUnit( shaderUnit );
    }
    m_shaders.clear();
}

void ShaderProgram::reCompileShader( EExecuteType inEt, const String& shaderPathString )
{
    CUL::String errorContent;
    reCompileShader( inEt, shaderPathString, true, errorContent );
}

void ShaderProgram::reCompileShader( EExecuteType inEt, const String& shaderPathString, bool assertOnErrors, CUL::String& errorMessage )
{
    if( inEt == EExecuteType::WaitForCompletion )
    {
        RunOnRenderThread::getInstance().RunWaitForResult(
            [this, inEt, &shaderPathString, assertOnErrors, &errorMessage]()
            {
                reCompileShaderImpl( inEt, shaderPathString, assertOnErrors, errorMessage );
            } );
    }
    else if( inEt == EExecuteType::ExecuteOnRenderThread )
    {
        m_tasks.addTask(
            [this, inEt, shaderPathString, assertOnErrors]()
            {
                CUL::String errorMessage;
                reCompileShaderImpl( inEt, shaderPathString, assertOnErrors, errorMessage );
            } );
    }
    else
    {
        reCompileShaderImpl( inEt, shaderPathString, assertOnErrors, errorMessage );
    }
}

void ShaderProgram::reCompileShaderImpl( EExecuteType inEt, const String& inShaderPathString, bool assertOnErrors, CUL::String& errorMessage )
{
    const CUL::FS::Path inShaderPath( inShaderPathString );
    const auto extension = inShaderPath.getExtension();
    CShaderTypes::ShaderType type = CShaderTypes::getShaderType( extension );

    std::unordered_map<CShaderTypes::ShaderType, CUL::String> shadersPaths;
    for( auto& [shaderType, shaderUnit] : m_shaders )
    {
        shadersPaths[shaderType] = shaderUnit->File->getPath();
        const std::uint32_t shaderId = shaderUnit->ID;
        getDevice()->deleteShaderUnit( shaderUnit );
        detachShader( inEt, shaderId );
        shaderUnit = nullptr;
    }

    shadersPaths[type] = inShaderPath;

    for( const auto& [shaderType, shaderPath] : shadersPaths )
    {
        compileShader( inEt, shaderPath, assertOnErrors, errorMessage );
    }
}

void ShaderProgram::compileShader( EExecuteType inEt, const String& shaderPath )
{
    CUL::String errorContent;
    compileShader( inEt, shaderPath, true, errorContent );
}

void ShaderProgram::compileShader( EExecuteType inEt, const String& shaderPath, bool assertOnErrors, CUL::String& errorMessage )
{
    ShaderUnit* su = getDevice()->createShaderUnit( shaderPath, assertOnErrors, errorMessage );
    if( errorMessage.empty() )
    {
        m_shaders[su->Type] = su;
        attachShader( inEt, su->ID );
        su->State = EShaderUnitState::Loaded;
    }
    else
    {
        su->State = EShaderUnitState::Error;
    }
}


void ShaderProgram::attachShader( EExecuteType inEt, std::uint32_t id )
{
    switch( inEt )
    {
        case LOGLW::EExecuteType::None:
            CUL::Assert::check( false, "ShaderProgram::attachShader: wrong execution type: None." );
            break;
        case LOGLW::EExecuteType::WaitForCompletion:
            RunOnRenderThread::getInstance().RunWaitForResult(
                [this, id]()
                {
                    getDevice()->attachShader( m_shaderProgramId, id );
                } );
            break;
        case LOGLW::EExecuteType::ExecuteOnRenderThread:
            CUL::Assert::check( false, "ShaderProgram::attachShader: cannot link in OnRenderThread mode." );
            break;
        case LOGLW::EExecuteType::Now:
            getDevice()->attachShader( m_shaderProgramId, id );
            break;
        default:
            CUL::Assert::check( false, "ShaderProgram::attachShader: wrong execution type." );
            break;
    }
}

void ShaderProgram::attachShaders( EExecuteType /*inEt*/ )
{
    for( const auto& su : m_shaders )
    {
        getDevice()->attachShader( m_shaderProgramId, su.second->ID );
    }
}

void ShaderProgram::detachShader( EExecuteType /*inEt*/, std::uint32_t id )
{
    getDevice()->dettachShader( m_shaderProgramId, id );
}

void ShaderProgram::link( EExecuteType inEt )
{
    if( inEt == EExecuteType::Now )
    {
        linkImpl();
    }
    else if( inEt == EExecuteType::WaitForCompletion )
    {
        RunOnRenderThread::getInstance().RunWaitForResult(
            [this]()
            {
                linkImpl();
            } );
    }
    else
    {
        CUL::Assert::check( false, "ShaderProgram::link wrong execute type!" );
    }
}

void ShaderProgram::linkImpl()
{
    getDevice()->linkProgram( m_shaderProgramId );
    getDevice()->useProgram( m_shaderProgramId );
    m_linked = true;
    m_uniformMapping.clear();  // Might have changed on link.

    const auto attributes = getDevice()->fetchProgramAttributeInfo( static_cast<std::int32_t>( m_shaderProgramId ) );
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

void ShaderProgram::create()
{
    RunOnRenderThread::getInstance().RunWaitForResult(
        [this]()
        {
            m_shaderProgramId = getDevice()->createProgram( getName() );
        } );
}

CShaderTypes::ShaderType ShaderProgram::getType() const
{
    return m_type;
}

void ShaderProgram::setUniform( EExecuteType inEt, const String& inName, UniformValue inValue )
{
    switch( inEt )
    {
        case EExecuteType::Now:
        {
            setUniformImpl( inName, inValue );
            break;
        }
        case EExecuteType::ExecuteOnRenderThread:
        {
            m_tasks.addTask(
                [this, inName, inValue]()
                {
                    setUniformImpl( inName, inValue );
                } );
            break;
        }
        default:
            CUL::Assert::check( false, "ShaderProgram::setUniform, not implemented execution type." );
    }
}

void ShaderProgram::setUniformImpl( const String& inName, UniformValue inValue )
{
    const auto it = m_uniformMapping.find( inName );

    ShaderVariable* sv{ nullptr };
    if( it == m_uniformMapping.end() )
    {
        const std::int32_t location = getDevice()->getUniformLocation( m_shaderProgramId, inName );

        if( location == -1 )
        {
            return;
        }

        UniformInfo ui;
        if (getDevice()->fetchUniformInfo(ui, m_shaderProgramId, inName))
        {
            if( ui.Type == DataType::NONE )
            {
                auto x = 0;
            }

            ShaderVariable currentSV;
            currentSV.Id = ui.ID;
            currentSV.Name = inName;
            currentSV.Size = ui.Size;
            currentSV.Type = ui.Type;
            currentSV.TypeName = ui.TypeName;
            currentSV.Value = inValue;
            m_uniformMapping[inName] = currentSV;
            sv = &m_uniformMapping[inName];
        }
    }
    else
    {
        sv = &it->second;
    }

    switch( sv->Type )
    {
        case DataType::FLOAT:
        {
            getDevice()->setUniformValue( sv->Id, std::get<float>( inValue ) );
            break;
        }
        case DataType::FLOAT_MAT4:
        {
            getDevice()->setUniformValue( sv->Id, std::get<glm::mat4>( inValue ) );
            break;
        }
        case DataType::FLOAT_VEC3:
        {
            getDevice()->setUniformValue( sv->Id, std::get<glm::vec3>( inValue ) );
            break;
        }
        case DataType::FLOAT_VEC4:
        {
            getDevice()->setUniformValue( sv->Id, std::get<glm::vec4>( inValue ) );
            break;
        }
        case DataType::SAMPLER_1D:
        {
            getDevice()->setUniformValue( sv->Id, std::get<std::int32_t>( inValue ) );
            break;
        }
        case DataType::SAMPLER_2D:
        {
            getDevice()->setUniformValue( sv->Id, std::get<std::int32_t>( inValue ) );
            break;
        }
        default:
            throw std::logic_error( "Method not implemented" );
    }

    sv->Value = inValue;
    sv->Applied = true;
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
    ZoneScoped;
    CUL::Assert::check( m_shaderProgramId != 0u, "NO SHADER IS BEING USED." );
    if( m_linked ) [[likely]]
    {
        getDevice()->useProgram( m_shaderProgramId );
        m_tasks.executeAll();
    }
    else
    {
        m_tasks.executeAll();
    }

    CUL::Assert::check( m_linked, "NO SHADER IS BEING USED." );
}

void ShaderProgram::runOnRenderingThread( std::function<void( void )> inFunction )
{
    m_tasks.addTask( inFunction );
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
