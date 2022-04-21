#pragma once

#include "gameengine/Import.hpp"
#include "gameengine/IUtilityUser.hpp"
#include "gameengine/Shader.hpp"

NAMESPACE_BEGIN( LOGLW )

class GAME_ENGINE_API IShaderFactory:
    public IUtilityUser
{
public:
    IShaderFactory() = default;
    virtual ~IShaderFactory() = default;

    virtual Shader* createShader( const CUL::FS::Path& filePath ) = 0;
    virtual Shader* getShader( const CUL::FS::Path& filePath ) = 0;

protected:
private:
    IShaderFactory( const IShaderFactory& arg ) = delete;
    IShaderFactory& operator=( const IShaderFactory& rhv ) = delete;
};

NAMESPACE_END( LOGLW )