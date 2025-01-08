#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"
#include "gameengine/IRenderDevice.hpp"

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
class TransformComponent;
class VertexArray;
class VertexBuffer;
struct VertexData;

class Sprite final: public IObject, public IUtilityUser
{
public:
    Sprite( Camera* camera, CUL::CULInterface* cul, IGameEngine* engine, bool forceLegacy = false );

    GAME_ENGINE_API void LoadImage( const CUL::FS::Path& imagePath, CUL::Graphics::IImageLoader* imageLoader );
    GAME_ENGINE_API void LoadImage( unsigned char* data, unsigned width, unsigned height, CUL::Graphics::IImageLoader*,
                                    unsigned textureId );

    GAME_ENGINE_API void setName( const CUL::String& name ) override;

    GAME_ENGINE_API void render() override;
    GAME_ENGINE_API const CUL::Graphics::ImageInfo& getImageInfo() const;
    GAME_ENGINE_API unsigned char* getData() const;

    GAME_ENGINE_API ~Sprite();

protected:
private:
    void init();
    bool m_initialized = false;

    enum class TaskType : short
    {
        CREATE_VAO = 1,
        CREATE_VBO,
        CREATE_VAO_VBO
    };

    void renderModern();
    void renderLegacy();
    void release();
    void fixAspectRatio();

    unsigned m_textureId = 0u;
    std::unique_ptr<VertexData> m_vertexData;

    TransformComponent* m_transformComponent = nullptr;

    Camera* m_camera = nullptr;
    CUL::CULInterface* m_cul = nullptr;

    CUL::Graphics::IImage* m_image = nullptr;

    class ShaderProgram* m_shaderProgram = nullptr;

    TextureInfo m_textureInfo;

    unsigned m_elementBufferId = 0u;
    VertexArray* m_vao = nullptr;
    VertexBuffer* m_vbo = nullptr;

    // Deleted:
    Sprite( const Sprite& arg ) = delete;
    Sprite( Sprite&& arg ) = delete;
    Sprite& operator=( const Sprite& rhv ) = delete;
    Sprite& operator=( Sprite&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )