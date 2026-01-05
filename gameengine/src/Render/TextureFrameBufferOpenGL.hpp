#pragma once

#include "gameengine/Render/ITextureFrameBuffer.hpp"
#include "gameengine/AttributeMeta.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/ITexture.hpp"

namespace LOGLW
{

class IGameEngine;
class IRenderDevice;
class ShaderProgram;

class TextureFrameBufferOpenGL: public ITextureFrameBuffer
{
public:
    TextureFrameBufferOpenGL( IRenderDevice& inRd, std::int32_t inWidth, std::int32_t inHeight );

    void beginCapture() override;
    void endCapture() override;
    void drawCapture() override;

    ~TextureFrameBufferOpenGL();

protected:
private:
    void render();
    const TexSize& getSize() const;
    TexID getID() const;

    IGameEngine& m_engine;
    IRenderDevice& m_renderDevice;
    VertexData m_vboData;
    TexSize m_size;
    std::uint32_t m_framebufferColor{ 0u };
    std::int32_t m_textureColor{ -1 };


    std::uint32_t m_framebufferDepthStencil{ 0u };

    TextureInfo m_ti;
    std::vector<std::uint32_t> m_fboData;
    ShaderProgram* m_shaderProgram{ nullptr };
    VertexArray* m_vao{ nullptr };

    CUL_NONCOPYABLE( TextureFrameBufferOpenGL )
};
}  // namespace LOGLW