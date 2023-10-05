#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"

NAMESPACE_BEGIN( LOGLW )

class Camera;
class Program;
class TransformComponent;
class VertexArray;
class VertexBuffer;
struct TextureInfo;

struct TexPixel
{
    uint8_t Red = 0u;
    uint8_t Green = 0u;
    uint8_t Blue = 0u;
    uint8_t Alpha = 0u;
};

class EditableTexture final: public IUtilityUser, public IObject{
public:
    EditableTexture() = delete;
    EditableTexture( const EditableTexture& ) = delete;
    EditableTexture( EditableTexture&& ) = delete;
    EditableTexture& operator=( const EditableTexture&) = delete;
    EditableTexture& operator=( EditableTexture&& ) = delete;

    GAME_ENGINE_API EditableTexture( Camera* camera, CUL::CULInterface* cul, IGameEngine* engine, bool forceLegacy );
    GAME_ENGINE_API void create( uint16_t width, uint16_t height );

    GAME_ENGINE_API void setPixelValue( uint16_t x, uint16_t y, const TexPixel& color );

    GAME_ENGINE_API ~EditableTexture();

protected:
private:
    void createImpl();
    void render() override;
    void updateTextureImpl();
    void renderModern();
    void renderLegacy();
    Program* m_shaderProgram = nullptr;
    std::unique_ptr<TextureInfo> m_ti;
    Camera* m_camera = nullptr;
    CUL::CULInterface* m_cul = nullptr;
    TransformComponent* m_transformComponent = nullptr;
    uint16_t m_width = 0u;
    uint16_t m_height = 0u;
    std::vector<TexPixel> m_pixelData;
    unsigned int m_textureId = 0u;
    bool m_create = false;
    bool m_needToApply = false;
    VertexArray* m_vao = nullptr;
    VertexBuffer* m_vbo = nullptr;
};

NAMESPACE_END( LOGLW )