#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"
#include "gameengine/AttributeMeta.hpp"

#include "CUL/Graphics/Color.hpp"
#include "CUL/IRegisteredObject.hpp"
#include "CUL/Math/Primitives/Quad.hpp"

#include "CUL/STL_IMPORTS/STD_atomic.hpp"

namespace CUL::Graphics
{
class IImage;
class IImageLoader;
class ImageInfo;
}

NAMESPACE_BEGIN( LOGLW )

class Camera;
class IGameEngine;
class TransformComponent;
class VertexArray;
struct TextureInfo;

struct GAME_ENGINE_API UV
{
    float X;
    float Y;
};

class Sprite final: public IUtilityUser, public IObject, public CUL::IRegisterdObject
{
public:
    GAME_ENGINE_API Sprite( IObject* parent, bool forceLegacy );

    GAME_ENGINE_API void LoadImage( const CUL::FS::Path& imagePath, CUL::Graphics::IImageLoader* imageLoader );
    GAME_ENGINE_API void LoadImage( unsigned char* data, unsigned width, unsigned height, CUL::Graphics::IImageLoader*,
                                    unsigned textureId );
    GAME_ENGINE_API const CUL::Graphics::ImageInfo& getImageInfo() const;
    GAME_ENGINE_API unsigned char* getData() const;
    GAME_ENGINE_API void setColor( const CUL::Graphics::ColorS& color );

    GAME_ENGINE_API const std::array<UV, 4>& getUV() const;
    GAME_ENGINE_API void setUV( const UV& inUV, std::size_t index );

    GAME_ENGINE_API ~Sprite();

protected:
    void onNameChange( const CUL::String& newName ) override;

private:
    void render() override;
    void init();
    void createBuffers();
    void createShaders();
    void setTransformationAndColor();
    void release();
    void updateBuffers();
    void updateBuffers_impl();
    void setSize( const glm::vec3& size );


    std::int32_t m_textureId{ 0 };
    std::array<std::array<float,8>, 4> m_vertexData;

    std::array<UV, 4> m_uvList;
    glm::mat4 m_model;

    VertexData m_vboData;

    TransformComponent* m_transformComponent{ nullptr };

    CUL::Graphics::ColorS m_color;
    bool m_unbindBuffersAfterDraw{ false };

    CUL::Graphics::IImage* m_image{ nullptr };
    std::unique_ptr<LOGLW::TextureInfo> m_textureInfo;

    // Deleted
private:
    Sprite( Sprite& ) = delete;
    Sprite( Sprite&& ) = delete;
    Sprite& operator=( const Sprite& ) = delete;
    Sprite& operator=( const Sprite&& ) = delete;
};

NAMESPACE_END( LOGLW )