#include "UI/WidgetTexture.hpp"
#include "gameengine/ExecuteType.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/Windowing/IWindow.hpp"
#include "gameengine/Windowing/WinSize.hpp"
#include "gameengine/VertexArray.hpp"
#include "RunOnRenderThread.hpp"
#include "CUL/STL_IMPORTS/STD_random.hpp"

namespace LOGLW
{

CWidgetTexture::CWidgetTexture()
{
    m_ti = std::make_unique<TextureInfo>();
    m_vertexData = std::make_unique<VertexData>();
    RunOnRenderThread::getInstance().RunWaitForResult(
        [this]()
        {
            init();
        } );
}

    std::uint8_t generate_byte()
{
    static std::random_device rd;     // seed source
    static std::mt19937 gen( rd() );  // Mersenne Twister engine
    static std::uniform_int_distribution<int> dist( 0, 255 );

    return static_cast<std::uint8_t>( dist( gen ) );
}

void CWidgetTexture::init()
{
    auto& gameEngine = getEngine();
    m_device = gameEngine.getDevice();

    const auto winSize = gameEngine.getMainWindow()->getSize();
    m_pixelData.resize( static_cast<size_t>( winSize.W * winSize.H ) );

    const std::size_t pixelSize = m_pixelData.size();

    std::size_t i = 0u;
    for( auto& color : m_pixelData )
    {
        color.Alpha = 128u;
        color.Red = 0.f;
        color.Green = 0u;
        color.Blue = i < pixelSize / 2u ? 0u : 255u;
        ++i;
    }

    m_shaderProgram = gameEngine.createProgram();

    m_shaderProgram->createFrom( EExecuteType::Now, { "embedded_shaders/camera_orto.vert", "embedded_shaders/camera_orto.frag" } );
    m_shaderProgram->link( EExecuteType::Now );
    m_shaderProgram->validate();

    m_textureId = static_cast<std::int32_t>( m_device->generateTexture() );

    m_ti->internalFormat = CUL::Graphics::PixelFormat::RGBA8;
    m_ti->dataFormat = CUL::Graphics::PixelFormat::RGBA;
    m_ti->size.x = static_cast<std::uint32_t>( winSize.W );
    m_ti->size.y = static_cast<std::uint32_t>( winSize.H );
    m_ti->data = (CUL::Graphics::DataType*)m_pixelData.data();
    m_ti->textureId = static_cast<decltype( m_ti->textureId )>( m_textureId );
    m_ti->initialized = true;

    m_device->setTextureData( m_textureId, *m_ti );
    m_device->setTextureParameter( m_textureId, TextureParameters::MAG_FILTER, TextureFilterType::LINEAR );
    m_device->setTextureParameter( m_textureId, TextureParameters::MIN_FILTER, TextureFilterType::LINEAR );

    m_vertexData->VAO = getVao()->getId();
    m_device->bindBuffer( BufferTypes::VERTEX_ARRAY, getVao()->getId() );
    m_device->enableVertexAttribArray( 0 );
    m_device->enableVertexAttribArray( 1 );

    struct UVx
    {
        float X;
        float Y;
    };

    std::array<UVx, 4> m_uvList;
    m_uvList[0] = { 0.f, 0.f };
    m_uvList[1] = { 1.f, 0.f };
    m_uvList[2] = { 1.f, 1.f };
    m_uvList[3] = { 0.f, 1.f };

    const float min = -1.f;
    const float max = 1.f;
    const float zVal = 0.f;
    std::array<std::array<float, 5>, 4> vertexData;
    vertexData = {
        min, max, 0.f, m_uvList[0].X, m_uvList[0].Y,
        max, max, 0.f, m_uvList[1].X, m_uvList[1].Y,
        max, min, 0.f, m_uvList[2].X, m_uvList[2].Y,
        min, min, 0.f, m_uvList[3].X, m_uvList[3].Y };

    std::vector<std::uint32_t> indices = {
        // note that we start from 0!
        0, 1, 2,  // first Triangle
        2, 3, 0   // second Triangle
    };

    VertexData m_vboData;
    m_vboData.Indices.createFrom( indices );

    FloatData fd;
    for( const auto& data : vertexData )
    {
        for( const auto& dataRow : data )
        {
            fd.push_back( dataRow );
        }
    }

    m_vboData.Data.createFrom( fd );

    m_vboData.Attributes.clear();

    AttributeMeta am;
    am.Name = "pos";
    am.Index = 0;
    am.Size = 3;
    am.Type = LOGLW::DataType::FLOAT;
    am.StrideBytes = 5 * sizeof( float );
    m_vboData.Attributes.emplace_back( am );

    am.Name = "tex";
    am.Index = 1;
    am.Size = 2;
    am.DataOffset = reinterpret_cast<void*>( 3 * sizeof( float ) );
    m_vboData.Attributes.emplace_back( am );

    m_vboData.primitiveType = LOGLW::PrimitiveType::TRIANGLES;

    m_vboData.VAO = getVao()->getId();

    getVao()->updateVertexBuffer( m_vboData );
    getVao()->setProgram( m_shaderProgram );

    getVao()->setName("WidgetTexture");
}

void CWidgetTexture::updatePixel( std::size_t x, std::size_t y, const S_RGBA& color )
{
    const auto winSize = getEngine().getMainWindow()->getSize();
    if( x >= static_cast<std::size_t>( winSize.W ) || y >= static_cast<std::size_t>( winSize.H ) )
    {
        return;
    }
    const auto index = y * static_cast<std::size_t>( winSize.W ) + x;
    m_pixelData[index] = color;
    m_updateData = true;
}

void CWidgetTexture::render()
{
    m_shaderProgram->enable();
    m_device->setActiveTextureUnit( ETextureUnitIndex::UNIT_0 );
    m_device->bindTexture( m_textureId );

    if( m_updateData )
    {
        m_device->updateTextureData( *m_ti, m_ti->data );
        m_updateData = false;
    }

    m_shaderProgram->setUniform( EExecuteType::Now, "texture1", 0 );

    getVao()->render();

    m_shaderProgram->disable();
}

CWidgetTexture::~CWidgetTexture()
{
}
}  // namespace LOGLW