#include "gameengine/Sprite.hpp"
#include "gameengine/IUtility.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Program.hpp"
#include "gameengine/Shader.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Components/TransformComponent.hpp"

#include "CUL/Graphics/IImageLoader.hpp"
#include "CUL/Math/Algorithms.hpp"
#include "CUL/Filesystem/FileFactory.hpp"
#include "CUL/Graphics/IImage.hpp"

#undef LoadImage

using namespace LOGLW;

Sprite::Sprite( Camera* camera, CUL::CULInterface* cul, IGameEngine* engine ) : IObject( engine ), m_camera( camera ), m_cul( cul )
{
    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    m_transformComponent->setSize( Pos( 2.f, 2.f, 2.f ) );
}

void Sprite::LoadImage( const CUL::FS::Path& imagePath, CUL::Graphics::IImageLoader* imageLoader )
{
    m_image = imageLoader->loadImage( imagePath );
}

void Sprite::LoadImage( CUL::Graphics::DataType* data, unsigned width, unsigned height, CUL::Graphics::IImageLoader* imageLoader,
                        unsigned )
{
    m_image = imageLoader->loadImage( (unsigned char*)data, width, height );
}

void Sprite::render()
{
    if( getUtility()->isLegacy() )
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
    if( !getUtility()->isLegacy() )
    {
        m_shaderProgram = std::make_unique<Program>();
        m_shaderProgram->initialize();
        m_shaderProgram->enable();

        const std::string vertexShaderSource =
#include "embedded_shaders/camera.vert"
            ;

        const std::string fragmentShaderSource =
#include "embedded_shaders/camera.frag"
            ;

        auto fragmentShaderFile = m_cul->getFF()->createRegularFileRawPtr( "embedded_shaders/camera.frag" );
        fragmentShaderFile->loadFromString( fragmentShaderSource );
        auto fragmentShader = new Shader( fragmentShaderFile );

        auto vertexShaderCode = m_cul->getFF()->createRegularFileRawPtr( "embedded_shaders/camera.vert" );
        vertexShaderCode->loadFromString( vertexShaderSource );
        auto vertexShader = new Shader( vertexShaderCode );

        m_shaderProgram->attachShader( vertexShader );
        m_shaderProgram->attachShader( fragmentShader );
        m_shaderProgram->link();
        m_shaderProgram->validate();
    }

    m_textureId = getUtility()->generateTexture();

    const auto& ii = getImageInfo();
    TextureInfo td;
    td.pixelFormat = CUL::Graphics::PixelFormat::RGBA;
    td.size = ii.canvasSize;
    td.data = getData();
    td.textureId = m_textureId;

    getUtility()->setTextureData( m_textureId, td );

    getUtility()->setTextureParameter( m_textureId, TextureParameters::MAG_FILTER, TextureFilterType::LINEAR );
    getUtility()->setTextureParameter( m_textureId, TextureParameters::MIN_FILTER, TextureFilterType::LINEAR );

    if( !getUtility()->isLegacy() )
    {
        m_vao = getUtility()->generateBuffer( LOGLW::BufferTypes::VERTEX_ARRAY );
        getUtility()->bindBuffer( BufferTypes::VERTEX_ARRAY, m_vao );
        getUtility()->enableVertexAttribArray( 0 );
        getUtility()->enableVertexAttribArray( 1 );

        m_vbo = getUtility()->generateBuffer( BufferTypes::ARRAY_BUFFER );

        const Pos& size = m_transformComponent->getSize();
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

        std::vector<float> dataVec;

        for( size_t i = 0; i < data.size(); ++i )
        {
            for( size_t j = 0; j < data[i].size(); ++j )
            {
                dataVec.push_back(data[i][j]);
            }
        }

        getUtility()->bufferData( m_vbo, dataVec, BufferTypes::ARRAY_BUFFER );

        VertexAttributePtrMeta meta;
        meta.vertexAttributeId = 0;
        meta.componentsPerVertexAttribute = 3;
        meta.vao = m_vao;
        meta.vbo = m_vbo;
        meta.dataType = DataType::FLOAT;
        meta.normalized = false;
        meta.stride = 5 * sizeof( float );

        getUtility()->vertexAttribPointer( meta );

        meta.vertexAttributeId = 1;
        meta.componentsPerVertexAttribute = 2;
        meta.offset = (void*)( 3 * sizeof( float ) );
        getUtility()->vertexAttribPointer( meta );

        getUtility()->unbindBuffer( LOGLW::BufferTypes::ARRAY_BUFFER );
        getUtility()->unbindBuffer( LOGLW::BufferTypes::ELEMENT_ARRAY_BUFFER );

        m_shaderProgram->enable();
        m_shaderProgram->setAttrib( "texture1", 0 );
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

    getUtility()->setActiveTexture( m_textureId );
    getUtility()->bindTexture( m_textureId );

    m_shaderProgram->enable();

    const glm::mat4 model = m_transformComponent->getModel();

    auto projectionMatrix = m_camera->getProjectionMatrix();
    auto viewMatrix = m_camera->getViewMatrix();

    m_shaderProgram->setAttrib( "projection", projectionMatrix );
    m_shaderProgram->setAttrib( "view", viewMatrix );
    m_shaderProgram->setAttrib( "model", model );

    getUtility()->bindBuffer( BufferTypes::ARRAY_BUFFER, m_vbo );

    getUtility()->drawArrays( m_vao, PrimitiveType::TRIANGLES, 0, 36 );

    m_shaderProgram->disable();

    getUtility()->bindBuffer( BufferTypes::ARRAY_BUFFER, 0 );
    getUtility()->bindBuffer( BufferTypes::VERTEX_ARRAY, 0 );
    getUtility()->bindTexture( 0u );
}

void Sprite::renderLegacy()
{
    if( !m_initialized )
    {
        init();
    }

    std::array<std::array<float, 3>, 4> values;
    values[3] = { 0.f, 0.f, 0.f };
    values[2] = { 1.f, 0.f, 0.f };
    values[1] = { 1.f, 1.f, 0.f };
    values[0] = { 0.f, 1.f, 0.f };
    QuadCUL colors = values;

    //pconst float denominator = 8.f;

#ifdef OLD_VER
    const auto& size = m_image->getImageInfo().size;
    float x0 = -(float)size.width / denominator;
    float x1 = (float)size.width / denominator;

    float y0 = -(float)size.height / denominator;
    float y1 = (float)size.height / denominator;
#else
    const Pos& size = m_transformComponent->getSize();
    float x0 = -size.x() / 2.f;
    float x1 = size.x() / 2.f;

    float y0 = -size.y() / 2.f;
    float y1 = size.y() / 2.f;

    float z0 = -size.z() / 2.f;
    //float z1 = size.z() / 2.f;
#endif


    values[0] = { x0, y0, z0 };
    values[1] = { x1, y0, z0 };
    values[2] = { x1, y1, z0 };
    values[3] = { x0, y1, z0 };
    QuadCUL positions = values;

    getUtility()->bindTexture( m_textureId );

    const auto position = m_transformComponent->getWorldPosition();

    getUtility()->matrixStackPush();
    getUtility()->translate( position );
    //static const auto type = CUL::MATH::Angle::Type::DEGREE;
    const auto rotation = m_transformComponent->getWorldRotation();
    getUtility()->rotate( rotation );
    getUtility()->draw( positions, colors );
    getUtility()->matrixStackPop();

    getUtility()->bindTexture( 0 );
}

Sprite::~Sprite()
{
    release();
}

void Sprite::release()
{
    m_shaderProgram.reset();
}