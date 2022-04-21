#pragma once

#include "gameengine/ITexture.hpp"
#include "gameengine/IUtilityUser.hpp"
#include "gameengine/IUtility.hpp"

#include "CUL/GenericUtils/DumbPtr.hpp"
#include "CUL/Math/Primitives/Quad.hpp"

NAMESPACE_BEGIN( CUL )
NAMESPACE_BEGIN( Graphics )
class IImage;
class IImageLoader;
NAMESPACE_END( Graphics )
NAMESPACE_BEGIN( FS )
class Path;
NAMESPACE_END( FS )
NAMESPACE_END( CUL )

NAMESPACE_BEGIN( LOGLW )

class TextureConcrete final:
    public ITexture,
    public IUtilityUser
{
public:
    TextureConcrete( IUtility* utility, CUL::Graphics::IImageLoader* imageLoader, const CUL::FS::Path& path );

    const TexSize& getSize() const override;
    TexID getID() const override;
    void render() override;

    ~TextureConcrete();
protected:
private:
    void calculateQuads();
    IUtility* m_utility = nullptr;
    CUL::Graphics::IImageLoader* m_il = nullptr;

    TextureInfo m_texInfo;
    unsigned int m_textureId = 0;
    CUL::GUTILS::DumbPtr<CUL::Graphics::IImage> m_image;
    CUL::Graphics::SSize2Di m_size;
    CUL::MATH::Primitives::Quad m_objectQuad;
    CUL::MATH::Primitives::Quad m_texQuad;

// Deleted:
    TextureConcrete( const TextureConcrete& arg ) = delete;
    TextureConcrete( TextureConcrete&& arg ) = delete;
    TextureConcrete& operator=( const TextureConcrete& arg ) = delete;
    TextureConcrete& operator==( TextureConcrete&& arg ) = delete;
};

NAMESPACE_END( LOGLW )