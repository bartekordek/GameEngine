#include "gameengine/Sprite.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Program.hpp"
#include "gameengine/Shader.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/AttributeMeta.hpp"

#include "CUL/Graphics/IImageLoader.hpp"
#include "CUL/Math/Algorithms.hpp"
#include "CUL/Filesystem/FileFactory.hpp"
#include "CUL/Graphics/IImage.hpp"

#undef LoadImage

using namespace LOGLW;

Sprite::Sprite( Camera* camera, CUL::CULInterface* cul, IGameEngine* engine, bool forceLegacy ): IObject( "Sprite", engine, forceLegacy ), m_camera( camera ), m_cul( cul )
{
    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    m_transformComponent->setSize( CUL::MATH::Point( 2.f, 2.f, 2.f ) );

    m_vertexData = std::make_unique<VertexData>();
}

void Sprite::LoadImage( const CUL::FS::Path& imagePath, CUL::Graphics::IImageLoader* imageLoader )
{
    m_image = imageLoader->loadImage( imagePath );
}

void Sprite::LoadImage( CUL::Graphics::DataType* data, unsigned width, unsigned height, CUL::Graphics::IImageLoader* imageLoader,
                        unsigned )
{
    m_image = imageLoader->loadImage( (unsigned char*)data, width, height );

    m_textureId = getDevice()->generateTexture();

    m_textureInfo.data = data;
    m_textureInfo.level = 0;
    m_textureInfo.border = 0;
    m_textureInfo.dataType = DataType::UNSIGNED_BYTE;
    m_textureInfo.pixelFormat = CUL::Graphics::PixelFormat::RGBA;
    m_textureInfo.textureId = m_textureId;
    m_textureInfo.size.width = width;
    m_textureInfo.size.height = height;
    m_textureInfo.initialized = true;

    getDevice()->setTextureData( m_textureId, m_textureInfo );
}

void Sprite::render()
{
    if( getDevice()->isLegacy() )
    {
        renderLegacy();
    }
    else
    {
        renderModern();
    }
}

const CUL::Graphics::ImageInfo& Sprite::getImageInfo() const
{
    return m_image->getImageInfo();
}

CUL::Graphics::DataType* Sprite::getData() const
{
    return m_image->getData();
}

void Sprite::init()
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

        m_shaderProgram->attachShader( vertexShader );
        m_shaderProgram->attachShader( fragmentShader );
        m_shaderProgram->link();
        m_shaderProgram->validate();
    }

    if( m_textureId == 0u )
    {
        m_textureId = getDevice()->generateTexture();
    }

    const auto& ii = getImageInfo();

    if( !m_textureInfo.initialized )
    {
        m_textureInfo.pixelFormat = CUL::Graphics::PixelFormat::RGBA;
        m_textureInfo.size = ii.canvasSize;
        m_textureInfo.data = getData();
        m_textureInfo.textureId = m_textureId;
        m_textureInfo.initialized = true;

        getDevice()->setTextureData( m_textureId, m_textureInfo );
    }

    getDevice()->setTextureParameter( m_textureId, TextureParameters::MAG_FILTER, TextureFilterType::LINEAR );
    getDevice()->setTextureParameter( m_textureId, TextureParameters::MIN_FILTER, TextureFilterType::LINEAR );

    if( !getDevice()->isLegacy() )
    {
        m_vao = getEngine().createVAO();
        m_vertexData->VAO = m_vao->getId();
        getDevice()->bindBuffer( BufferTypes::VERTEX_ARRAY, m_vao->getId() );
        getDevice()->enableVertexAttribArray( 0 );
        getDevice()->enableVertexAttribArray( 1 );


        const CUL::MATH::Point& size = m_transformComponent->getSize();
        float x0 = -size.x() / 2.f;
        float x1 = size.x() / 2.f;

        float y0 = -size.y() / 2.f;
        float y1 = size.y() / 2.f;

        float z0 = -size.z() / 2.f;
        //float z1 = size.z() / 2.f;

        std::array<std::array<float, 5>, 6> data;
        data[0] = { x0, y1, z0, 0.0f, 0.0f };
        data[1] = { x1, y1, z0, 1.0f, 0.0f };
        data[2] = { x1, y0, z0, 1.0f, 1.0f };
        data[3] = { x1, y0, z0, 1.0f, 1.0f };
        data[4] = { x0, y0, z0, 0.0f, 1.0f };
        data[5] = { x0, y1, z0, 0.0f, 0.0f };
        for( size_t i = 0; i < data.size(); ++i )
        {
            for( size_t j = 0; j < data[i].size(); ++j )
            {
                m_vertexData->vertices.push_back( data[i][j] );
            }
        }

        m_vbo = getEngine().createVBO( *m_vertexData.get() );
        m_vertexData->VBO = m_vbo->getId();
        getDevice()->bufferData( m_vbo->getId(), m_vertexData->vertices, BufferTypes::ARRAY_BUFFER );

        m_vertexData->Attributes.push_back( AttributeMeta( "pos", 0, 3, DataType::FLOAT, false, 5 * sizeof( float ), nullptr ) );
        m_vertexData->Attributes.push_back( AttributeMeta( "uvs", 1, 2, DataType::FLOAT, false, 5 * sizeof( float ), reinterpret_cast<void*>( 3 * sizeof( float ) ) ) );

        getDevice()->vertexAttribPointer( *m_vertexData.get() );

        getDevice()->unbindBuffer( LOGLW::BufferTypes::ARRAY_BUFFER );
        getDevice()->unbindBuffer( LOGLW::BufferTypes::ELEMENT_ARRAY_BUFFER );

        m_shaderProgram->enable();
        m_shaderProgram->setUniform( "texture1", 0 );
        m_shaderProgram->disable();
    }
    m_initialized = true;
}

void Sprite::renderModern()
{
    if( !m_initialized )
    {
        init();
    }

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

void Sprite::renderLegacy()
{
    if( !m_initialized )
    {
        init();
    }

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
    //float z1 = size.z() / 2.f;

    values[0] = { x0, y0, z0, 0.f, 0.f, 1.f };
    values[1] = { x1, y0, z0, 0.f, 0.f, 1.f };
    values[2] = { x1, y1, z0, 0.f, 0.f, 1.f };
    values[3] = { x0, y1, z0, 0.f, 0.f, 1.f };
    QuadCUL positions = values;

    getDevice()->bindTexture( m_textureId );

    const auto position = m_transformComponent->getPositionAbsolut();

    getDevice()->matrixStackPush();
    getDevice()->translate( position );
    //static const auto type = CUL::MATH::Angle::Type::DEGREE;
    const auto rotation = m_transformComponent->getRotationAbsolute();
    getDevice()->rotate( rotation );
    getDevice()->draw( positions, colors );
    getDevice()->matrixStackPop();

    getDevice()->bindTexture( 0 );
}

Sprite::~Sprite()
{
    release();
}

void Sprite::release()
{
    getEngine().removeProgram( m_shaderProgram );
}