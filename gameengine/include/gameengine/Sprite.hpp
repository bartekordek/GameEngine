#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"

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

class GAME_ENGINE_API Sprite final: public IObject, public IUtilityUser
{
public:
    Sprite( Camera* camera, CUL::CULInterface* cul, IGameEngine* engine );

    void LoadImage( const CUL::FS::Path& imagePath, CUL::Graphics::IImageLoader* imageLoader );
    void LoadImage( unsigned char* data, unsigned width, unsigned height, CUL::Graphics::IImageLoader*, unsigned textureId );

    unsigned m_textureId = 0u;

    void render() override;
    const CUL::Graphics::ImageInfo& getImageInfo() const;
    unsigned char* getData() const;

    ~Sprite();

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

    TransformComponent* m_transformComponent = nullptr;

    Camera* m_camera = nullptr;
    CUL::CULInterface* m_cul = nullptr;

    CUL::Graphics::IImage* m_image = nullptr;

    class Program* m_shaderProgram = nullptr;

    TextureInfo m_textureInfo;

    unsigned m_elementBufferId = 0u;
    unsigned m_vao = 0u;
    unsigned m_vbo = 0u;

    // Deleted:
    Sprite( const Sprite& arg ) = delete;
    Sprite( Sprite&& arg ) = delete;
    Sprite& operator=( const Sprite& rhv ) = delete;
    Sprite& operator=( Sprite&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )