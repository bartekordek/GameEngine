#include "TextureConcrete.hpp"
#include "CUL/Graphics/IImageLoader.hpp"
#include "CUL/Graphics/IImage.hpp"
#include "gameengine/IUtility.hpp"

using namespace LOGLW;

TextureConcrete::TextureConcrete(
    IUtility* utility,
    CUL::Graphics::IImageLoader* imageLoader,
    const CUL::FS::Path& path ):
    m_utility( utility ),
    m_il( imageLoader )
{
    m_image = m_il->loadImage( path );
    auto info = m_image->getImageInfo();

    m_texInfo.data = m_image->getData();
    m_texInfo.size.width = info.size.width;
    m_texInfo.size.height = info.size.height;
    m_texInfo.pixelFormat = info.pixelFormat;

    m_textureId = utility->generateTexture();
    utility->bindTexture( m_textureId );
    utility->setTextureData( m_textureId, m_texInfo );
    utility->setTextureParameter( m_textureId, TextureParameters::MAG_FILTER, TextureFilterType::LINEAR );
    utility->setTextureParameter( m_textureId, TextureParameters::MIN_FILTER, TextureFilterType::LINEAR );
    utility->bindTexture( 0 );

    calculateQuads();
}

const TexSize& TextureConcrete::getSize() const
{
    return m_texInfo.size;
}

TexID TextureConcrete::getID() const
{
    return m_textureId;
}

void TextureConcrete::calculateQuads()
{
    if( m_textureId )
    {
        auto imageWidth = m_texInfo.size.width;
        auto imageHeight = m_texInfo.size.height;

        auto textureWidth = m_texInfo.size.width;
        auto textureHeight = m_texInfo.size.height;

        auto texTop = (float) imageHeight / (float) textureHeight;
        auto texBottom = 0.f;

        auto texLeft = 0.f;
        auto texRight = (float) imageWidth / (float) textureWidth;

        auto quadWidth = static_cast<float>(imageWidth);
        auto quadHeight = static_cast<float>(imageHeight);

        //TODO:
        ////Handle clipping
        //if( clip != nullptr )
        //{
        //    //Texture coordinates
        //    texLeft = clip->x / mTextureWidth;
        //    texRight = (clip->x + clip->w) / mTextureWidth;
        //    texTop = clip->y / mTextureHeight;
        //    texBottom = (clip->y + clip->h) / mTextureHeight;

        //    //Vertex coordinates
        //    quadWidth = clip->w;
        //    quadHeight = clip->h;
        //}

        m_objectQuad[0] = {      0.0f,       0.0f, 0.0f };
        m_objectQuad[1] = { quadWidth,       0.0f, 0.0f };
        m_objectQuad[2] = { quadWidth, quadHeight, 0.0f };
        m_objectQuad[3] = {      0.0f, quadHeight, 0.0f };

        m_texQuad[0] = {  texLeft,    texTop, 0.0f };
        m_texQuad[1] = { texRight,    texTop, 0.0f };
        m_texQuad[2] = { texRight, texBottom, 0.0f };
        m_texQuad[3] = {  texLeft, texBottom, 0.0f };
    }
}

void TextureConcrete::render()
{
    if( m_textureId )
    {
        m_utility->bindTexture( m_textureId );
        m_utility->draw( m_objectQuad, m_texQuad );
    }
}

TextureConcrete::~TextureConcrete()
{
    m_utility->freeTexture( m_textureId );
}