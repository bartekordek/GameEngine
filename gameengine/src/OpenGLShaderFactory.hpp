#pragma once

#include "gameengine/IShaderFactory.hpp"
#include "gameengine/IProgramFactory.hpp"
#include "CUL/STL_IMPORTS/STD_map.hpp"
#include "gameengine/IUtility.hpp"

NAMESPACE_BEGIN( LOGLW )

using ShaderPtr = CUL::GUTILS::DumbPtr<Shader>;
using ProgramPtr = CUL::GUTILS::DumbPtr<Program>;

using MyString = CUL::String;
using ShaderMap = std::map<MyString, ShaderPtr>;
using ProgramMap = std::map<unsigned int, ProgramPtr>;

class IGameEngine;
class OpenGLShaderFactory final:
    public IShaderFactory,
    public IProgramFactory
{
public:
    explicit OpenGLShaderFactory( IGameEngine* wrapper );
    ~OpenGLShaderFactory();

    Shader* createShader( const CUL::FS::Path& filePath ) override;
    Program* createProgram() override;
    Shader* getShader( const CUL::FS::Path& filePath ) override;

protected:
private:
    OpenGLShaderFactory( const OpenGLShaderFactory& arg ) = delete;
    OpenGLShaderFactory& operator=( const OpenGLShaderFactory& rhv ) = delete;

    bool shaderExist( const CUL::FS::Path& filePath ) const;

    Shader* addShader( const CUL::FS::Path& filePath );

    void log( const String& text );

    mutable ShaderMap m_shaders;
    ProgramMap m_programs;
    IGameEngine& m_gameEngine;
    CUL::CULInterface* m_culInterface = nullptr;
};

NAMESPACE_END( LOGLW )