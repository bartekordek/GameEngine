#include "gameengine/Sprite.hpp"
#include "gameengine/IUtility.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Program.hpp"
#include "gameengine/Shader.hpp"
#include "gameengine/Camera.hpp"

#include "CUL/Graphics/IImageLoader.hpp"
#include "CUL/Math/Algorithms.hpp"
#include "CUL/Filesystem/FileFactory.hpp"
#include "CUL/Graphics/IImage.hpp"

#undef LoadImage

using namespace LOGLW;

Sprite::Sprite( Camera* camera, CUL::CULInterface* cul ): m_camera(camera), m_cul( cul )
{
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

void Sprite::renderModern()
{
    if( !m_initialized )
    {
        init();
    }

    getUtility()->setActiveTexture( m_textureId );
    getUtility()->bindTexture( m_textureId );

    m_shaderProgram->enable();

    glm::mat4 model = glm::mat4( 1.0f );  // make sure to initialize matrix to identity matrix first
    const Pos& position = getWorldPosition();
    glm::vec3 m_pos = position.toGlmVec();
    model = glm::translate( model, m_pos );

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

    QuadCUL quad1;

    /*
    00   10
    10   11
    */

    std::array<std::array<float, 3>, 4> values;
    values[3] = { 0.f, 0.f, 0.f };
    values[2] = { 1.f, 0.f, 0.f };
    values[1] = { 1.f, 1.f, 0.f };
    values[0] = { 0.f, 1.f, 0.f };
    quad1.setData( values );

    QuadCUL quad2;
    const auto& size = m_image->getImageInfo().size;

    float x0 = -(float)size.width / 2.f;
    float x1 = (float)size.width / 2.f;

    float y0 = -(float)size.height / 2.f;
    float y1 = (float)size.height / 2.f;

    /*
    00 10
    01 11
    */

    values[0] = { x0, y0, 0.f };
    values[1] = { x1, y0, 0.f };
    values[2] = { x1, y1, 0.f };
    values[3] = { x0, y1, 0.f };

    quad2.setData( values );

    getUtility()->bindTexture( m_textureId );

    getUtility()->matrixStackPush();
    getUtility()->translate( getWorldPosition() );
    static const auto type = CUL::MATH::Angle::Type::DEGREE;
    getUtility()->rotate( getWorldAngle( CUL::MATH::EulerAngles::YAW ).getValueF( type ), 0.f, 0.f, 1.f );
    getUtility()->rotate( getWorldAngle( CUL::MATH::EulerAngles::PITCH ).getValueF( type ), 0.f, 1.f, 0.f );
    getUtility()->rotate( getWorldAngle( CUL::MATH::EulerAngles::ROLL ).getValueF( type ), 1.f, 0.f, 0.f );
    getUtility()->draw( quad2, quad1 );
    getUtility()->matrixStackPop();

    getUtility()->bindTexture( 0 );
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

        std::vector<float> data = {
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  1.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f };

        getUtility()->bufferData( m_vbo, data, BufferTypes::ARRAY_BUFFER );

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

Sprite::~Sprite()
{
}