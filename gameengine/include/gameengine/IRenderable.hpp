#pragma once

#include "gameengine/IEngineUser.hpp"
#include "CUL/Filesystem/Path.hpp"

NAMESPACE_BEGIN( LOGLW )

class Program;
class Shader;

class GAME_ENGINE_API IRenderable: public IEngineUser
{
public:
    IRenderable( IGameEngine* engine );

    void setDisableRenderOnMyOwn( bool disable );

    virtual void render() = 0;


    void loadShader( const CUL::FS::Path& shaderPath );

    virtual ~IRenderable();


    IRenderable( const IRenderable& value ) = delete;
    IRenderable( IRenderable&& value ) = delete;
    IRenderable& operator=( const IRenderable& value ) = delete;
    IRenderable& operator=( IRenderable&& value ) = delete;
protected:
private:
    Program* m_program = nullptr;
    Shader* m_vertexShader = nullptr;
    Shader* m_fragmentShader = nullptr;
};

NAMESPACE_END( LOGLW )