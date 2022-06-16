#pragma once

#include "gameengine/IUtilityUser.hpp"

#include "CUL/Filesystem/Path.hpp"
#include "CUL/Filesystem/IFile.hpp"

NAMESPACE_BEGIN( LOGLW )

class IGameEngine;

class GAME_ENGINE_API Shader final:
    private IUtilityUser
{
public:
    Shader( IGameEngine& engine, CUL::FS::IFile* file );

    unsigned int getId() const;
    void useShader() const;
    void reload();
    const CUL::FS::Path& getPath() const;

    ~Shader();

protected:
private:
    void create();
    void release();

    IGameEngine& m_engine;
    CUL::GUTILS::DumbPtr<CUL::FS::IFile> m_shaderCode;
    unsigned int m_id = 0;

    Shader( const Shader& arg ) = delete;
    Shader( Shader&& arg ) = delete;
    Shader& operator=( const Shader& rhv ) = delete;
    Shader& operator=( Shader&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )