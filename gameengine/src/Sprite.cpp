#include "gameengine/Sprite.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/ExecuteType.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/IObject.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/AttributeMeta.hpp"
#include "RunOnRenderThread.hpp"
#include "CUL/Graphics/IImageLoader.hpp"
#include "CUL/Proifling/Profiler.hpp"

#include "CUL/IMPORT_GLM.hpp"

using namespace LOGLW;

Sprite::Sprite( IObject* parent, bool forceLegacy ):
    IObject( "", forceLegacy )
{
    m_transformComponent = getTransform();
    setParent( parent );

    m_uvList[0] = { 0.f, 0.f };
    m_uvList[1] = { 1.f, 0.f };
    m_uvList[2] = { 1.f, 1.f };
    m_uvList[3] = { 0.f, 1.f };

    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    constexpr float size = 4.f;
    m_transformComponent->setSize( CUL::MATH::Point( size, size, 0.f ) );
    // TODO: add normals
    setSize( { size, size, 0 } );

    RunOnRenderThread::getInstance().RunWaitForResult(
        [this]()
        {
            init();
        } );
    m_transformComponent->changeSizeDelegate.addDelegate(
        [this]()
        {
            const auto size = m_transformComponent->getSize();
            setSize( size.toGlmVec() );
            updateBuffers();
        } );

    m_textureInfo = std::make_unique<LOGLW::TextureInfo>();
}

void Sprite::onNameChange( const String& newName )
{
    IObject::onNameChange( newName );
    getDevice()->setObjectName( EObjectType::TEXTURE, static_cast<std::uint32_t>( m_textureId ), getName() + "/texture" );
}

void Sprite::LoadImage( const CUL::FS::Path& imagePath, CUL::Graphics::IImageLoader* imageLoader )
{
    m_image = imageLoader->loadImage( imagePath );
    const auto& ii = getImageInfo();
    m_textureInfo->pixelFormat = CUL::Graphics::PixelFormat::RGBA;
    m_textureInfo->size = ii.canvasSize;
    m_textureInfo->data = getData();
    m_textureInfo->textureId = static_cast<decltype( m_textureInfo->textureId )>( m_textureId );
    m_textureInfo->initialized = true;

    getDevice()->setTextureData( m_textureId, *m_textureInfo );
    getDevice()->setTextureParameter( m_textureId, TextureParameters::MAG_FILTER, TextureFilterType::LINEAR );
    getDevice()->setTextureParameter( m_textureId, TextureParameters::MIN_FILTER, TextureFilterType::LINEAR );

    const float maxImensionSize = std::max( m_textureInfo->size.width, m_textureInfo->size.height );
    const float newRectWidth = m_textureInfo->size.width / maxImensionSize;
    const float newRectHeight = m_textureInfo->size.height / maxImensionSize;

    m_transformComponent->setSize( { newRectWidth, newRectHeight, 0.f } );
}

void Sprite::LoadImage(
    CUL::Graphics::DataType* data,
    unsigned width,
    unsigned height,
    CUL::Graphics::IImageLoader* imageLoader,
    unsigned )
{
    m_image = imageLoader->loadImage( (unsigned char*)data, width, height );

    m_textureId = static_cast<std::int32_t>( getDevice()->generateTexture() );

    m_textureInfo->data = data;
    m_textureInfo->level = 0;
    m_textureInfo->border = 0;
    m_textureInfo->dataType = DataType::UNSIGNED_BYTE;
    m_textureInfo->pixelFormat = CUL::Graphics::PixelFormat::RGBA;
    m_textureInfo->textureId = m_textureId;
    m_textureInfo->size.width = width;
    m_textureInfo->size.height = height;
    m_textureInfo->initialized = true;

    getDevice()->setTextureData( m_textureId, *m_textureInfo );
}

void Sprite::setColor( const CUL::Graphics::ColorS& color )
{
    m_color = color;
    glm::vec4 colorVec;
    colorVec.x = m_color.getRF();
    colorVec.y = m_color.getGF();
    colorVec.z = m_color.getBF();
    colorVec.w = m_color.getAF();
}

void Sprite::init()
{
    if( getDevice()->isLegacy() )
    {
    }
    else
    {
        getVao()->setProgram( getProgram() );
        m_textureId = getDevice()->generateTexture();
        createBuffers();
        createShaders();

        setTransformationAndColor();
    }
}

void Sprite::createBuffers()
{
    updateBuffers();
}

void Sprite::updateBuffers()
{
    /*RunOnRenderThread::getInstance().RunWaitForResult(
        [this]()
        {
            updateBuffers_impl();
        } );*/
    updateBuffers_impl();
}

const CUL::Graphics::ImageInfo& Sprite::getImageInfo() const
{
    return m_image->getImageInfo();
}

CUL::Graphics::DataType* Sprite::getData() const
{
    return m_image->getData();
}

void Sprite::setUV( const UV& inUV, std::size_t index )
{
    m_uvList[index] = inUV;
    const auto size = m_transformComponent->getSize();
    setSize( size.toGlmVec() );
    updateBuffers_impl();
}

void Sprite::updateBuffers_impl()
{
    std::vector<std::uint32_t> indices = {
        // note that we start from 0!
        0, 1, 2,  // first Triangle
        2, 3, 0   // second Triangle
    };
    m_vboData.Indices.createFrom( indices );

    FloatData fd;
    for( const auto& data : m_vertexData )
    {
        for( const auto& dataRow: data )
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
}

void Sprite::setSize( const glm::vec3& size )
{
    m_vertexData = { 
        0.0f, size.y, 0.0f, m_uvList[0].X, m_uvList[0].Y,
      size.x, size.y, 0.0f, m_uvList[1].X, m_uvList[1].Y,
      size.x,   0.0f, 0.0f, m_uvList[2].X, m_uvList[2].Y,
        0.0f,   0.0f, 0.0f, m_uvList[3].X, m_uvList[3].Y
    };
}

const std::array<UV, 4>& Sprite::getUV() const
{
    return m_uvList;
}

void Sprite::createShaders()
{
    CUL::String errorContent;
    ShaderProgram::ShadersData sd;
    sd.FragmentShader = "embedded_shaders/camera.frag";
    sd.VertexShader = "embedded_shaders/camera.vert";

    getProgram()->createFrom( EExecuteType::Now, sd );
}

void Sprite::render()
{
    ProfilerScope( "Sprite::render" );

    getDevice()->setActiveTextureUnit( ETextureUnitIndex::UNIT_0 );
    getDevice()->bindTexture( m_textureId );

    if( getDevice()->isLegacy() || getForceLegacy() )
    {
        // getDevice()->draw( m_shape, m_transformComponent->getModel(), m_color );
    }
    else
    {
        setTransformationAndColor();
        getVao()->render();

        if( m_unbindBuffersAfterDraw == true )
        {
            getProgram()->disable();
        }
    }
}

void Sprite::setTransformationAndColor()
{
    ProfilerScope( "Sprite::setTransformationAndColor" );

    const Camera& camera = getEngine().getCamera();
    const glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    const glm::mat4 viewMatrix = camera.getViewMatrix();

    const glm::mat4 model = m_transformComponent->getModel();

    ShaderProgram* shaderProgram = getProgram();
    shaderProgram->runOnRenderingThread(
        [this, shaderProgram, projectionMatrix, viewMatrix, model]()
        {
            shaderProgram->setUniform( EExecuteType::Now, "projection", projectionMatrix );
            shaderProgram->setUniform( EExecuteType::Now, "view", viewMatrix );
            shaderProgram->setUniform( EExecuteType::Now, "model", model );
            shaderProgram->setUniform( EExecuteType::Now, "color", m_color.getVec4() );
            shaderProgram->setUniform( EExecuteType::Now, "texture1", 0 );
        } );
}

Sprite::~Sprite()
{
    RunOnRenderThread::getInstance().RunWaitForResult(
        [this]()
        {
            release();
        } );
}

void Sprite::release()
{
}
