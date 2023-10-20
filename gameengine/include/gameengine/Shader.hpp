#pragma once

#include "gameengine/IUtilityUser.hpp"
#include "gameengine/ShaderTypes.hpp"

#include "CUL/IName.hpp"
#include "CUL/IRegisteredObject.hpp"
#include "CUL/Filesystem/Path.hpp"
#include "CUL/Filesystem/IFile.hpp"

#include "CUL/STL_IMPORTS/STD_vector.hpp"

NAMESPACE_BEGIN( LOGLW )

class IGameEngine;
class Program;

class GAME_ENGINE_API Shader final: private IUtilityUser, public CUL::IName, public CUL::IRegisterdObject
{
public:
    Shader( IGameEngine& engine, CUL::FS::IFile* file );

    unsigned int getId() const;
    void useShader() const;
    void reload();
    const CUL::FS::Path& getPath() const;

    void addUsedFrom( Program* inProgram );
    void removeUsedFrom( Program* inProgram );
    size_t getUsedFromCount() const;

    ShaderTypes::ShaderType getType() const;

    ~Shader();

protected:
private:
    void create();
    void release();

    ShaderTypes::ShaderType m_type{ ShaderTypes::ShaderType::Unkown };
    std::vector<Program*> m_usedFromList;

    IGameEngine& m_engine;
    CUL::GUTILS::DumbPtr<CUL::FS::IFile> m_shaderCode;
    std::uint32_t m_id = 0;

    Shader( const Shader& arg ) = delete;
    Shader( Shader&& arg ) = delete;
    Shader& operator=( const Shader& rhv ) = delete;
    Shader& operator=( Shader&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )