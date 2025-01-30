#pragma once

#include "CUL/Filesystem/Path.hpp"
#include "CUL/Graphics/ITexture.hpp"
#include <CUL/STL_IMPORTS/STD_memory.hpp>
#include <CUL/STL_IMPORTS/STD_mutex.hpp>

struct SDL_Texture;

NAMESPACE_BEGIN( SDL2W )

using ITexture = CUL::Graphics::ITexture;
using Vector3Dd = CUL::MATH::Vector3Dd;

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable: 4820 )
#endif

class TextureSDL:
    public ITexture
{
public:
    TextureSDL();
    TextureSDL( const TextureSDL& val ) = delete;
    virtual ~TextureSDL();

    TextureSDL& operator=( const TextureSDL& val ) = delete;

    SDL_Texture* getTexture() const;
    void setTexture(
        const SDL_Texture* texture,
        const CUL::FS::Path& path );
    const Vector3Dd& getSize() const override;
    const CUL::FS::Path& getPath() const override;
protected:
private:
    SDL_Texture* m_texture = nullptr;
    Vector3Dd m_size;
    CUL::FS::Path m_path;
};

#ifdef _MSC_VER
#pragma warning( pop )
#endif

NAMESPACE_END( SDL2W )