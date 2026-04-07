#pragma once

#include "gameengine/UI/WidgetEditable.hpp"
#include "gameengine/Render/PixelFormats.hpp"
#include "CUL/STL_IMPORTS/STD_memory.hpp"

namespace LOGLW
{
class IRenderDevice;
class ShaderProgram;
struct TextureInfo;
struct VertexData;

class CWidgetTexture: public IWidgetEditable
{
public:
    CWidgetTexture();
    void render();

    void updatePixel( std::size_t x, std::size_t y, const S_RGBA& color ) override;

    ~CWidgetTexture();

    CUL_NONCOPYABLE( CWidgetTexture )
protected:
private:
    void init();
    bool m_updateData{ false };
    ShaderProgram* m_shaderProgram{ nullptr };
    std::int32_t m_textureId{ -1 };
    std::unique_ptr<TextureInfo> m_ti;
    std::vector<S_RGBA> m_pixelData;
    std::unique_ptr<VertexData> m_vertexData;
    IRenderDevice* m_device{ nullptr };
};
}  // namespace LOGLW