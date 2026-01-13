#include "Render/TextureFrameBufferOpenGL.hpp"
#include "gameengine/ExecuteType.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/VertexArray.hpp"
#include "DeviceOpenGL.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"

namespace LOGLW
{
TextureFrameBufferOpenGL::TextureFrameBufferOpenGL( IRenderDevice& inRd, std::int32_t inWidth, std::int32_t inHeight ):
    m_engine( *IGameEngine::getInstance() ),
    m_renderDevice( inRd ),
    m_size( inWidth, inHeight )
{
    glGenFramebuffers( 1, &m_framebufferColor );
    glBindFramebuffer( GL_FRAMEBUFFER, m_framebufferColor );
    m_renderDevice.setObjectName( EObjectType::FRAMEBUFFER, m_framebufferColor, "Main Frame Buffer" );

    m_textureColor = m_renderDevice.generateTexture();
    m_renderDevice.setObjectName( EObjectType::TEXTURE, m_textureColor, "Main Frame Buffer Texture" );
    m_renderDevice.bindTexture( m_textureColor );

    const std::size_t pixelCount{ static_cast<std::size_t>(inWidth * inHeight) };
    m_fboData.resize( pixelCount );
    std::memset( m_fboData.data(), 0u, pixelCount );

    m_ti.size = m_size;
    m_ti.textureId = m_textureColor;
    //m_ti.data = m_fboData.data();
    m_ti.pixelFormat = CUL::Graphics::PixelFormat::RGB;
    m_renderDevice.setTextureData( m_textureColor, m_ti );

    m_renderDevice.setTextureParameter( m_textureColor, TextureParameters::MAG_FILTER, TextureFilterType::LINEAR );
    m_renderDevice.setTextureParameter( m_textureColor, TextureParameters::MIN_FILTER, TextureFilterType::LINEAR );
    m_renderDevice.setTextureParameter( m_textureColor, TextureParameters::WRAP_S, TextureFilterType::REPEAT );
    m_renderDevice.setTextureParameter( m_textureColor, TextureParameters::WRAP_T, TextureFilterType::REPEAT );

    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_framebufferColor, 0 );

    glGenRenderbuffers( 1, &m_framebufferDepthStencil );
    glBindRenderbuffer( GL_RENDERBUFFER, m_framebufferDepthStencil );
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_size.width, m_size.height );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_framebufferDepthStencil );
                  // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if( glCheckFramebufferStatus( GL_FRAMEBUFFER ) != GL_FRAMEBUFFER_COMPLETE )
    {
        CUL::Assert::check(false, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    }
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    CUL::String errorContent;
    ShaderProgram::ShadersData sd;
    sd.FragmentShader = "embedded_shaders/fbo.frag";
    sd.VertexShader = "embedded_shaders/fbo.vert";
    auto engine = IGameEngine::getInstance();
    m_shaderProgram = engine->createProgram();
    m_shaderProgram->createFrom( EExecuteType::Now, sd );

    m_vao = m_engine.createVAO();
    m_vao->toggleRenderOnMyOwn( false );

    const float tr{ 0.5f };
    std::vector<float> quadVertices = { 
        -tr, tr, 0.0f, 1.0f,
        -tr,-tr, 0.0f, 0.0f,
         tr,-tr, 1.0f, 0.0f,
         tr, tr, 1.0f, 1.0f,
        -tr, tr, 0.0f, 1.0f,
         tr,-tr, 1.0f, 0.0f };

    m_vboData.Data.createFrom( quadVertices );

    AttributeMeta am;
    am.Name = "aPos";
    am.Index = 0;
    am.Size = 2;
    am.Type = LOGLW::DataType::FLOAT;
    am.StrideBytes = sizeof(float) * 4u;
    m_vboData.Attributes.emplace_back( am );

    am.Name = "aTexCoords";
    am.Index = 1;
    am.Size = 2;
    am.Type = LOGLW::DataType::FLOAT;
    am.DataOffset = reinterpret_cast<void*>( 2 * sizeof( float ) );
    m_vboData.Attributes.emplace_back( am );

    m_vboData.primitiveType = LOGLW::PrimitiveType::TRIANGLES;
    m_vboData.VAO = m_vao->getId();
    m_vao->updateVertexBuffer( m_vboData );
    m_vao->setName( "fbo_vao" );
}

void TextureFrameBufferOpenGL::beginCapture()
{
    glBindFramebuffer( GL_FRAMEBUFFER, m_framebufferColor );
    glEnable( GL_DEPTH_TEST );

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void TextureFrameBufferOpenGL::endCapture()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0u );

    glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glDisable( GL_DEPTH_TEST );
}

void TextureFrameBufferOpenGL::drawCapture()
{
    m_shaderProgram->enable();
    m_vao->bind();

    m_renderDevice.bindTexture( m_textureColor );
    m_vao->getVertexBuffer( 0 )->render();
}

const TexSize& TextureFrameBufferOpenGL::getSize() const
{
    return m_size;
}

TexID TextureFrameBufferOpenGL::getID() const
{
    return m_textureColor;
}

TextureFrameBufferOpenGL::~TextureFrameBufferOpenGL()
{
}

}  // namespace LOGLW