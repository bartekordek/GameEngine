#include "TextureSDL.hpp"

#include "CUL/GenericUtils/SimpleAssert.hpp"

#include "SDL2Wrapper/IMPORT_SDL_renderer.hpp"

using namespace SDL2W;

TextureSDL::TextureSDL()
{
}

SDL_Texture* TextureSDL::getTexture() const
{
    return m_texture;
}

void TextureSDL::setTexture( const SDL_Texture* texture, const CUL::FS::Path& path )
{
    m_texture = const_cast<SDL_Texture*>( texture );
    int w = 0;
    int h = 0;
    const auto sdlQuerySuccess = SDL_QueryTexture(
        m_texture,
        nullptr,
        nullptr,
        &w,
        &h );
    CUL::Assert::simple(
        0 == sdlQuerySuccess,
        "Cannot initialize SDL subsystem" );
    m_size.setX( static_cast<unsigned>( w ) );
    m_size.setY( static_cast<unsigned>( h ) );
    m_path = path;
}

const Vector3Dd& TextureSDL::getSize() const
{
    return m_size;
}

const CUL::FS::Path& TextureSDL::getPath() const
{
    return m_path;
}

TextureSDL::~TextureSDL()
{
    SDL_DestroyTexture( m_texture );
    m_texture = nullptr;
}