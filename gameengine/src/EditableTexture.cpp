#include "gameengine/EditableTexture.h"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/IGameEngine.hpp"
#include "RunOnRenderThread.hpp"

LOGLW::EditableTexture::EditableTexture( Camera* camera, CUL::CULInterface* cul, IGameEngine* engine, bool forceLegacy )
    : IObject( "EditableTexture", engine, forceLegacy ), m_camera( camera ), m_cul( cul )
{
    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    m_transformComponent->setSize( CUL::MATH::Point( 8.f, 6.f, 2.f ) );
    m_vertexData = std::make_unique<VertexData>();

    m_ti = std::make_unique<TextureInfo>();

    IName::AfterNameChangeCallback = [this]( const CUL::String& newName )
    {
        getEngine().addPostRenderTask(
            [this, newName]()
            {
                m_shaderProgram->setName( getName() + "::program" );
                m_vao->setName( getName() + "::vao" );
                m_vbo->setName( getName() + "::vbo" );
                getDevice()->setObjectName( EObjectType::TEXTURE, m_textureId, getName() + "::texture" );
            } );
    };
}

void LOGLW::EditableTexture::create( uint16_t width, uint16_t height )
{
    m_width = width;
    m_height = height;

    m_create = true;
    m_pixelData.resize( static_cast<size_t>( width * height ) );
    m_transformComponent->setSize( CUL::MATH::Point( m_width, m_height, 2.f ) );

    for(auto& color: m_pixelData)
    {
        color.Alpha = 255u;
        color.Red = 0u;
        color.Green = 255u;
        color.Blue = 0u;
    }

    m_imageInfo = std::make_unique<CUL::Graphics::ImageInfo>();
    m_imageInfo->size = { width, height };
}

void LOGLW::EditableTexture::setPixelValue( uint16_t x, uint16_t y, const TexPixel& color )
{
    const size_t offset = x + y * m_width;
    //TexPixel* targetValue = (TexPixel*)( (size_t)m_ti->data + offset );
    //*targetValue = color;
    m_pixelData[offset] = color;
    m_needToApply = true;
}

void LOGLW::EditableTexture::render()
{
    if( m_create )
    {
        init();
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

void LOGLW::EditableTexture::init()
{
    if( !getDevice()->isLegacy() )
    {
        m_shaderProgram = getEngine().createProgram();

        const std::string vertexShaderSource =
#include "embedded_shaders/camera.vert"
            ;

        const std::string fragmentShaderSource =
#include "embedded_shaders/camera.frag"
            ;

        auto fragmentShader = getEngine().createShader( "embedded_shaders/camera.frag", fragmentShaderSource );
        auto vertexShader = getEngine().createShader( "embedded_shaders/camera.vert", vertexShaderSource );

        throw std::logic_error( "Method not implemented" );
        /*m_shaderProgram->attachShader( vertexShader );
        m_shaderProgram->attachShader( fragmentShader );
        m_shaderProgram->link();
        m_shaderProgram->validate();*/
    }

    if( m_textureId == 0u )
    {
        m_textureId = getDevice()->generateTexture();
    }

    if( !m_ti->initialized )
    {
        m_ti->pixelFormat = CUL::Graphics::PixelFormat::RGBA;
        m_ti->size = { m_width, m_height };
        m_ti->data = getData();
        m_ti->textureId = m_textureId;
        m_ti->initialized = true;
        
        getDevice()->setTextureData( m_textureId, *m_ti );
    }

    getDevice()->setTextureParameter( m_textureId, TextureParameters::MAG_FILTER, TextureFilterType::LINEAR );
    getDevice()->setTextureParameter( m_textureId, TextureParameters::MIN_FILTER, TextureFilterType::LINEAR );

    if( !getDevice()->isLegacy() )
    {
        m_vao = getEngine().createVAO();
        m_vao->setDisableRenderOnMyOwn( true );
        m_vertexData->VAO = m_vao->getId();
        getDevice()->bindBuffer( BufferTypes::VERTEX_ARRAY, m_vao->getId() );
        getDevice()->enableVertexAttribArray( 0 );
        getDevice()->enableVertexAttribArray( 1 );

        const CUL::MATH::Point& size = m_transformComponent->getSize();
        float x0 = 0.f;
        float x1 = size.x();

        float y0 = 0.f;
        float y1 = size.y();

        float z0 = -size.z() / 2.f;
        // float z1 = size.z() / 2.f;

        std::array<std::array<float, 5>, 6> data;
        data[0] = { x0, y1, z0, 0.0f, 0.0f };
        data[1] = { x1, y1, z0, 1.0f, 0.0f };
        data[2] = { x1, y0, z0, 1.0f, 1.0f };
        data[3] = { x1, y0, z0, 1.0f, 1.0f };
        data[4] = { x0, y0, z0, 0.0f, 1.0f };
        data[5] = { x0, y1, z0, 0.0f, 0.0f };
        std::vector<float> tmp;
        for( size_t i = 0; i < data.size(); ++i )
        {
            for( size_t j = 0; j < data[i].size(); ++j )
            {
                tmp.push_back( data[i][j] );
            }
        }

        m_vertexData->Data.createFrom( tmp );

        m_vbo = getEngine().createVBO( *m_vertexData.get() );
        m_vbo->setDisableRenderOnMyOwn( true );
        m_vertexData->VBO = m_vbo->getId();
        getDevice()->bufferData( m_vbo->getId(), m_vertexData->Data, BufferTypes::ARRAY_BUFFER );

        m_vertexData->Attributes.push_back( AttributeMeta( "pos", 0, 3, DataType::FLOAT, false, 5 * sizeof( float ), nullptr ) );
        m_vertexData->Attributes.push_back( AttributeMeta( "uvs", 1, 2, DataType::FLOAT, false, 5 * sizeof( float ), reinterpret_cast<void*>( 3 * sizeof( float ) ) ) );

        getDevice()->vertexAttribPointer( *m_vertexData.get() );

        getDevice()->unbindBuffer( LOGLW::BufferTypes::ARRAY_BUFFER );
        getDevice()->unbindBuffer( LOGLW::BufferTypes::ELEMENT_ARRAY_BUFFER );

        throw std::logic_error( "Method not implemented" );
        //m_shaderProgram->enable();
        //m_shaderProgram->setUniform( "texture1", 0 );
        //m_shaderProgram->disable();
    }
    m_initialized = true;
}

CUL::Graphics::DataType* LOGLW::EditableTexture::getData() const
{
    return (CUL::Graphics::DataType*)m_pixelData.data();
}

const CUL::Graphics::ImageInfo& LOGLW::EditableTexture::getImageInfo() const
{
    return *m_imageInfo;
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
    float x0 = 0.f;
    float x1 = size.x();

    float y0 = 0.f;
    float y1 = size.y();

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

void LOGLW::EditableTexture::renderModern()
{
    if( !m_initialized )
    {
        init();
    }

    getDevice()->setActiveTextureUnit( ETextureUnitIndex::UNIT_0 );
    getDevice()->bindTexture( m_textureId );

    throw std::logic_error( "Method not implemented" );
    //m_shaderProgram->enable();

    const glm::mat4 model = m_transformComponent->getModel();

    auto projectionMatrix = m_camera->getProjectionMatrix();
    auto viewMatrix = m_camera->getViewMatrix();

    m_shaderProgram->setUniform( "projection", projectionMatrix );
    m_shaderProgram->setUniform( "view", viewMatrix );
    m_shaderProgram->setUniform( "model", model );

    getDevice()->bindBuffer( BufferTypes::VERTEX_ARRAY, m_vao->getId() );
    getDevice()->bindBuffer( BufferTypes::ARRAY_BUFFER, m_vbo->getId() );

    getDevice()->drawArrays( m_vao->getId(), PrimitiveType::TRIANGLES, 0, 6 );

    throw std::logic_error( "Method not implemented" );
    //m_shaderProgram->disable();

    getDevice()->bindBuffer( BufferTypes::ARRAY_BUFFER, 0 );
    getDevice()->bindBuffer( BufferTypes::VERTEX_ARRAY, 0 );
    getDevice()->bindTexture( 0u );
}

void LOGLW::EditableTexture::updateTextureImpl()
{
    getDevice()->setTextureData( m_textureId, *m_ti );
    //getDevice()->updateTextureData( *m_ti, m_pixelData.data() );
}

LOGLW::EditableTexture::~EditableTexture()
{
}
