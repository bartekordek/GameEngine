#include "gameengine/Sprite.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/AttributeMeta.hpp"
#include "RunOnRenderThread.hpp"

#include "CUL/Graphics/IImageLoader.hpp"
#include "CUL/Math/Algorithms.hpp"
#include "CUL/Filesystem/FileFactory.hpp"
#include "CUL/Graphics/IImage.hpp"

#undef LoadImage

using namespace LOGLW;

Sprite::Sprite( Camera* camera, CUL::CULInterface* cul, IGameEngine* engine, bool forceLegacy )
    : IObject( "Sprite", engine, forceLegacy ), m_camera( camera ), m_cul( cul )
{
    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    m_transformComponent->setSize( CUL::MATH::Point( 2.f, 2.f, 2.f ) );
    m_vertexData = std::make_unique<VertexData>();

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

void Sprite::setName( const CUL::String& name )
{
    IObject::setName( name );
    if( m_shaderProgram )
    {
        m_shaderProgram->setName( getName() + "::shader_program" );
    }
    if( m_vao )
    {
        m_vao->setName( getName() + "::shader_program::vao" );
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

void Sprite::init()
{
    if( !getDevice()->isLegacy() )
    {
        m_shaderProgram = getEngine().createProgram();

        m_shaderProgram->loadShader( "embedded_shaders/camera.frag" );
        m_shaderProgram->loadShader( "embedded_shaders/camera.vert" );
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
        m_vao->setDisableRenderOnMyOwn( true );
        m_vertexData->VAO = m_vao->getId();
        getDevice()->bindBuffer( BufferTypes::VERTEX_ARRAY, m_vao->getId() );
        getDevice()->enableVertexAttribArray( 0 );
        getDevice()->enableVertexAttribArray( 1 );
        fixAspectRatio();
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

        m_vbo = getEngine().createVBO( *m_vertexData );
        m_vbo->setDisableRenderOnMyOwn( true );
        m_vertexData->VBO = m_vbo->getId();
        getDevice()->bufferData( m_vbo->getId(), m_vertexData->Data, BufferTypes::ARRAY_BUFFER );

        m_vertexData->Attributes.push_back( AttributeMeta( "pos", 0, 3, DataType::FLOAT, false, 5 * sizeof( float ), nullptr ) );
        m_vertexData->Attributes.push_back( AttributeMeta( "uvs", 1, 2, DataType::FLOAT, false, 5 * sizeof( float ), reinterpret_cast<void*>( 3 * sizeof( float ) ) ) );

        getDevice()->vertexAttribPointer( *m_vertexData );

        getDevice()->unbindBuffer( LOGLW::BufferTypes::ARRAY_BUFFER );
        getDevice()->unbindBuffer( LOGLW::BufferTypes::ELEMENT_ARRAY_BUFFER );

        m_shaderProgram->enable();
        m_shaderProgram->setUniform( "texture1", 0 );
        m_shaderProgram->disable();
    }
    m_initialized = true;
}

void Sprite::fixAspectRatio()
{
    const auto size = m_transformComponent->getSize();
    const auto maximumValue = std::max( size.x(), size.y() );

    const auto imageSize = m_image->getImageInfo().size;
    const auto imageRatio = static_cast<float>(imageSize.width) / static_cast<float>(imageSize.height);

    LOGLW::TransformComponent::Pos newSize;
    if( imageSize.width == imageSize.height )
    {
        newSize.x() = maximumValue;
        newSize.y() = maximumValue;
    }
    else if( imageRatio > 1.f )
    {
        newSize.x() = maximumValue;
        newSize.y() = maximumValue / imageRatio;
    }
    else
    {
        newSize.x() = maximumValue / imageRatio;
        newSize.y() = maximumValue;
    }

    m_transformComponent->setSize( newSize );
}

Sprite::~Sprite()
{
    release();
}

void Sprite::release()
{
    getEngine().removeProgram( m_shaderProgram );
}
