#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/Render/PixelFormats.hpp"

NAMESPACE_BEGIN( CUL )
class CULInterface;

NAMESPACE_BEGIN( Graphics )
class IImageLoader;
class IImage;
class ImageInfo;
NAMESPACE_END( Graphics )

NAMESPACE_END( CUL )

NAMESPACE_BEGIN( LOGLW )

class Camera;
class Program;
class TransformComponent;
class ShaderProgram;
class VertexArray;
class VertexBuffer;
struct TextureInfo;
struct VertexData;


class EditableTexture final: public IUtilityUser,
    public IObject
{
public:
    EditableTexture() = delete;
    EditableTexture( const EditableTexture& ) = delete;
    EditableTexture( EditableTexture&& ) = delete;
    EditableTexture& operator=( const EditableTexture&) = delete;
    EditableTexture& operator=( EditableTexture&& ) = delete;

    GAME_ENGINE_API EditableTexture( Camera* camera, CUL::CULInterface* cul, bool forceLegacy );
    GAME_ENGINE_API void create( uint16_t width, uint16_t height );

    GAME_ENGINE_API void setPixelValue( std::int32_t x, std::int32_t y, const S_RGBA_I& color );
    GAME_ENGINE_API void setPixelValue( std::int32_t x, std::int32_t y, const S_RGBA_F& color );
    GAME_ENGINE_API const CUL::Graphics::ImageInfo& getImageInfo() const;

    GAME_ENGINE_API ~EditableTexture();

protected:
    void onNameChange( const CUL::StringWr& newName ) override;

private:
    void init();
    void render() override;
    void updateTextureImpl();
    void renderModern();
    void renderLegacy();
    unsigned char* getData() const;
    ShaderProgram* m_shaderProgram = nullptr;
    std::unique_ptr<TextureInfo> m_ti;
    Camera* m_camera = nullptr;
    CUL::CULInterface* m_cul = nullptr;
    TransformComponent* m_transformComponent = nullptr;
    std::int32_t m_width = -1;
    std::int32_t m_height = -1;
    std::vector<S_RGBA_I> m_pixelData;
    std::int32_t m_textureId = -1;
    bool m_create = false;
    bool m_needToApply = false;

    std::unique_ptr<VertexData> m_vertexData;
    bool m_initialized = false;
    std::unique_ptr<CUL::Graphics::ImageInfo> m_imageInfo;
};

NAMESPACE_END( LOGLW )