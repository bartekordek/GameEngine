#include "gameengine/EditableTexture.h"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/Program.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/VertexArray.hpp"

LOGLW::EditableTexture::EditableTexture( Camera* camera, CUL::CULInterface* cul, IGameEngine* engine, bool forceLegacy )
    : IObject( "EditableTexture", engine, forceLegacy ), m_camera( camera ), m_cul( cul )
{
    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
}

void LOGLW::EditableTexture::create( uint16_t width, uint16_t height )
{
    m_width = width;
    m_height = height;

    m_create = true;
    m_pixelData.resize( width * height );
}

void LOGLW::EditableTexture::setPixelValue( uint16_t x, uint16_t y, const TexPixel& color )
{
    const size_t offset = m_height * y + x;
    TexPixel* targetValue = (TexPixel*)( (size_t)m_ti->data + offset );
    *targetValue = color;
    m_needToApply = true;
}

void LOGLW::EditableTexture::render()
{
    if( m_create )
    {
        createImpl();
        m_create = false;
    }

    if( m_needToApply )
    {
        updateTextureImpl();
        m_needToApply = false;
    }

    if( getDevice()->isLegacy() )
    {
        renderLegacy();
    }
    else
    {
        renderModern();
    }
}

void LOGLW::EditableTexture::createImpl()
{
    m_textureId = getDevice()->generateTexture();
    m_ti = std::make_unique<TextureInfo>();
    m_ti->textureId = m_textureId;
    m_ti->pixelFormat = CUL::Graphics::PixelFormat::RGBA;
    m_ti->dataType = DataType::UNSIGNED_BYTE;
    m_ti->data = m_pixelData.data();
    getDevice()->setTextureData( m_textureId, *m_ti );
    // getDevice()->setTextureParameter( m_texId, TextureParameters::MAG_FILTER, TextureFilterType::LINEAR );
    // getDevice()->setTextureParameter( m_texId, TextureParameters::MIN_FILTER, TextureFilterType::LINEAR );
}

void LOGLW::EditableTexture::updateTextureImpl()
{
    getDevice()->updateTextureData( *m_ti, m_ti->data );
}

void LOGLW::EditableTexture::renderModern()
{
    getDevice()->setActiveTextureUnit( ETextureUnitIndex::UNIT_0 );
    getDevice()->bindTexture( m_textureId );

    m_shaderProgram->enable();

    const glm::mat4 model = m_transformComponent->getModel();

    auto projectionMatrix = m_camera->getProjectionMatrix();
    auto viewMatrix = m_camera->getViewMatrix();

    m_shaderProgram->setUniform( "projection", projectionMatrix );
    m_shaderProgram->setUniform( "view", viewMatrix );
    m_shaderProgram->setUniform( "model", model );

    getDevice()->bindBuffer( BufferTypes::VERTEX_ARRAY, m_vao->getId() );
    getDevice()->bindBuffer( BufferTypes::ARRAY_BUFFER, m_vbo->getId() );

    getDevice()->drawArrays( m_vao->getId(), PrimitiveType::TRIANGLES, 0, 6 );

    m_shaderProgram->disable();

    getDevice()->bindBuffer( BufferTypes::ARRAY_BUFFER, 0 );
    getDevice()->bindBuffer( BufferTypes::VERTEX_ARRAY, 0 );
    getDevice()->bindTexture( 0u );
}

void LOGLW::EditableTexture::renderLegacy()
{
    QuadData values;
    values[3] = { 0.f, 0.f, 0.f, 0.f, 0.f, 1.f };
    values[2] = { 1.f, 0.f, 0.f, 0.f, 0.f, 1.f };
    values[1] = { 1.f, 1.f, 0.f, 0.f, 0.f, 1.f };
    values[0] = { 0.f, 1.f, 0.f, 0.f, 0.f, 1.f };
    QuadCUL colors = values;

    const CUL::MATH::Point& size = m_transformComponent->getSize();
    float x0 = -size.x() / 2.f;
    float x1 = size.x() / 2.f;

    float y0 = -size.y() / 2.f;
    float y1 = size.y() / 2.f;

    float z0 = -size.z() / 2.f;
    // float z1 = size.z() / 2.f;

    values[0] = { x0, y0, z0, 0.f, 0.f, 1.f };
    values[1] = { x1, y0, z0, 0.f, 0.f, 1.f };
    values[2] = { x1, y1, z0, 0.f, 0.f, 1.f };
    values[3] = { x0, y1, z0, 0.f, 0.f, 1.f };
    QuadCUL positions = values;

    getDevice()->bindTexture( m_textureId );

    const auto position = m_transformComponent->getPositionAbsolut();

    getDevice()->matrixStackPush();
    getDevice()->translate( position );
    // static const auto type = CUL::MATH::Angle::Type::DEGREE;
    const auto rotation = m_transformComponent->getRotationAbsolute();
    getDevice()->rotate( rotation );
    getDevice()->draw( positions, colors );
    getDevice()->matrixStackPop();

    getDevice()->bindTexture( 0 );
}

LOGLW::EditableTexture::~EditableTexture()
{
}