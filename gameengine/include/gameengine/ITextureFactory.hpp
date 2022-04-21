#pragma once

#include "gameengine/Import.hpp"

NAMESPACE_BEGIN( CUL )
NAMESPACE_BEGIN( FS )
class Path;
NAMESPACE_END( FS )
NAMESPACE_END( CUL )

NAMESPACE_BEGIN( LOGLW )

class ITexture;

class GAME_ENGINE_API ITextureFactory
{
public:
    ITextureFactory() = default;
    virtual ~ITextureFactory() = default;

    virtual ITexture* createTexture( const CUL::FS::Path& path, const bool rgba = false ) = 0;

protected:
private:
    ITextureFactory( const ITextureFactory& value ) = delete;
    ITextureFactory& operator=( const ITextureFactory& value ) = delete;
};

NAMESPACE_END( LOGLW )