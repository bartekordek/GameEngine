#include "OpenGLShaderFactory.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"
#include "CUL/Log/ILogContainer.hpp"
#include "gameengine/IGameEngine.hpp"

using OpenGLShaderFactory = LOGLW::OpenGLShaderFactory;
using ShaderPtr = LOGLW::ShaderPtr;
using IFile = LOGLW::IFile;
using Shader = LOGLW::Shader;
using Program = LOGLW::Program;

OpenGLShaderFactory::OpenGLShaderFactory( LOGLW::IGameEngine* wrapper ):
    m_gameEngine( *wrapper ),
    m_culInterface( wrapper->getCul() )
{
}

Shader* OpenGLShaderFactory::createShader( const CUL::FS::Path& filePath )
{
    auto result = getShader( filePath );
    if( result )
    {
        return result;
    }
    else
    {
        return addShader( filePath );
    }
}

Program* OpenGLShaderFactory::createProgram()
{
    log( "OpenGLShaderFactory::createProgram()" );

    //Program* result = new ProgramConcrete( IUtilityUser::getUtility(), *this );
    //m_programs[result->getProgramId()] = result;
    //return result;
    CUL::Assert::simple( false, "TODO: IMPLEMENT!" );
    return nullptr;
}

bool OpenGLShaderFactory::shaderExist( const CUL::FS::Path& filePath ) const
{
    ShaderMap::iterator it = m_shaders.find( filePath );
    return m_shaders.end() != it;
}

Shader* OpenGLShaderFactory::getShader( const CUL::FS::Path& filePath )
{
    return m_shaders[ filePath ].get();
}

Shader* OpenGLShaderFactory::addShader( const CUL::FS::Path& filePath )
{
    log( "OpenGLShaderFactory::addShader: creating: " + filePath );
    auto shaderFile = m_culInterface->getFF()->createFileFromPath( filePath );
    auto shader = new Shader( shaderFile );
    m_shaders[ filePath ] = shader;
    return shader;
}

OpenGLShaderFactory::~OpenGLShaderFactory()
{
    m_shaders.clear();
    m_programs.clear();
}

void OpenGLShaderFactory::log( const String& text )
{
    m_culInterface->getLogger()->log(text);
}