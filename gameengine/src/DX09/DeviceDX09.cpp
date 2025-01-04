#include "DX09/DeviceDX09.hpp"

#if defined(GAME_ENGINE_WINDOWS)

using namespace LOGLW;

DeviceDX09::DeviceDX09() : IRenderDevice( false )
{
}

DeviceDX09::~DeviceDX09()
{
}

void* DeviceDX09::getNativeDevice()
{
    return nullptr;
}

bool DeviceDX09::isLegacy()
{
    return false;
}

LOGLW::ContextInfo DeviceDX09::initContextVersion( SDL2W::IWindow* /*window*/ )
{
    LOGLW::ContextInfo result;
    return result;
}

void DeviceDX09::createDescriptorHeaps()
{
}

void DeviceDX09::createRenderTargetViews()
{
}

void DeviceDX09::createRootSignature()
{
}

void DeviceDX09::createShaders()
{
}

void DeviceDX09::createVertexBuffer()
{
}

void DeviceDX09::createSwapChain()
{
}

void DeviceDX09::createCommandQueue()
{
}

void DeviceDX09::setViewportSize()
{
}

void DeviceDX09::setScissorRect()
{
}

void DeviceDX09::initInterfaces()
{
}

void DeviceDX09::createDXDevice()
{
}

void DeviceDX09::setUniformValue( int uniformLocation, const glm::vec2& val )
{
}

void DeviceDX09::setUniformValue( int uniformLocation, const glm::vec3& val )
{
}

void DeviceDX09::setUniformValue( int uniformLocation, const glm::vec4& val )
{
}

void DeviceDX09::setUniformValue( int uniformLocation, const glm::mat2& val )
{
}

void DeviceDX09::setUniformValue( int uniformLocation, const glm::mat3& val )
{
}

void DeviceDX09::setUniformValue( int uniformLocation, const glm::mat4& val )
{
}

void DeviceDX09::setProjectionAndModelToIdentity()
{
}

void DeviceDX09::clearColorAndDepthBuffer()
{
}

void DeviceDX09::createQuad( float scale /*= 1.0f */ )
{
}

void DeviceDX09::clearColorTo( const ColorS color )
{
}

void DeviceDX09::clearBuffer( const ClearMasks mask )
{
}

unsigned int DeviceDX09::generateVertexArray( const int size /*= 1 */ )
{
    return 0u;
}

void DeviceDX09::setClientState( ClientStateTypes cs, bool enabled )
{
}

void DeviceDX09::texCoordPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer )
{
}

void DeviceDX09::vertexPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer )
{
}

void DeviceDX09::setVertexArrayClientState( const bool enable )
{
}

void DeviceDX09::setColorClientState( bool enable )
{
}

void DeviceDX09::deleteBuffer( BufferTypes bufferType, unsigned& id )
{
}

void DeviceDX09::enableVertexAttribiute( unsigned programId, const String& attribName )
{
}

void DeviceDX09::disableVertexAttribiute( unsigned programId, const String& attribName )
{
}

int DeviceDX09::getAttribLocation( unsigned programId, const String& attribName )
{
    return 0;
}

int DeviceDX09::getUniformLocation( unsigned programId, const String& attribName )
{
    return 0;
}

void DeviceDX09::unbindBuffer( const BufferTypes bufferType )
{
}

void DeviceDX09::bindBuffer( const BufferTypes bufferType, unsigned bufferId )
{
}

unsigned int DeviceDX09::generateBuffer( const BufferTypes type, const int size /*= 1 */ )
{
    return 0u;
}

void DeviceDX09::drawElements( const PrimitiveType type, const std::vector<unsigned int>& data )
{
}

void DeviceDX09::drawElements( const PrimitiveType type, const std::vector<float>& data )
{
}

void DeviceDX09::drawElementsFromLastBuffer( const PrimitiveType primitiveType, const DataType dataType, unsigned count )
{
}

void DeviceDX09::drawArrays( unsigned vaoId, const PrimitiveType primitiveType, unsigned first, unsigned count )
{
}

void DeviceDX09::vertexAttribPointer( const VertexData& meta )
{
}

void DeviceDX09::enableVertexAttribArray( unsigned attributeId )
{
}

void DeviceDX09::setVertexPointer( int coordinatesPerVertex, DataType dataType, int stride, const void* data )
{
}

void DeviceDX09::draw( const QuadCUL& quad, const QuadCUL& texQuad )
{
}

void DeviceDX09::draw( const QuadCUL& quad, const ColorS& color )
{
}

void DeviceDX09::draw( const QuadCUL& quad, const Point& translation, const CUL::MATH::Rotation& rotation, const ColorS& color )
{
}

void DeviceDX09::draw( const QuadCUL& quad, const QuadColors& color )
{
}

void DeviceDX09::draw( const QuadData& quad, const ColorS& color )
{
}

void DeviceDX09::draw( const QuadData& quad, const std::array<ColorS, 4>& color )
{
}

void DeviceDX09::draw( const TriangleCUL& triangle, const ColorS& color )
{
}

void DeviceDX09::draw( const TriangleCUL& quad, const std::array<ColorS, 4>& color )
{
}

void DeviceDX09::draw( const TriangleData& values, const std::array<ColorS, 3>& color )
{
}

void DeviceDX09::draw( const LineData& values, const ColorS& color )
{
}

void DeviceDX09::draw( const LineData& values, const LineColors& color )
{
}

void DeviceDX09::draw( const CUL::MATH::Primitives::Line& values, const ColorS& color )
{
}

void DeviceDX09::draw( const Point& position, const ColorS& color )
{
}

void DeviceDX09::draw( const QuadCUL& quad, const glm::mat4& model, const ColorS& color )
{
}

void DeviceDX09::draw( const TriangleCUL& triangle, const glm::mat4& model, const ColorS& color )
{
}

void DeviceDX09::translate( const Point& point )
{
}

void DeviceDX09::translate( const float x, const float y, const float z )
{
}

void DeviceDX09::rotate( const CUL::MATH::Rotation& rotation )
{
}

void DeviceDX09::rotate( const float angleDeg, const float x /*= 0.0f*/, const float y /*= 0.0f*/, const float z /*= 0.0f */ )
{
}

void DeviceDX09::scale( const CUL::MATH::Vector3Df& scale )
{
}

void DeviceDX09::scale( const float scale )
{
}

void DeviceDX09::setDepthTest( const bool enabled )
{
}

void DeviceDX09::setBackfaceCUll( const bool enabled )
{
}

void DeviceDX09::setTexuring( const bool enabled )
{
}

void DeviceDX09::setActiveTextureUnit( ETextureUnitIndex textureUnitIndex )
{
}

void DeviceDX09::bindTexture( const unsigned int textureId )
{
}

void DeviceDX09::setTextureParameter( uint8_t textureId, const TextureParameters type, const TextureFilterType val )
{
}

void DeviceDX09::updateTextureData( const TextureInfo& ti, void* data )
{
}

void DeviceDX09::matrixStackPush()
{
}

void DeviceDX09::matrixStackPop()
{
}

void DeviceDX09::toggleDebugOutput( bool enable )
{
}

void DeviceDX09::checkLastCommandForErrors()
{
}

void DeviceDX09::enableDebugLayers()
{
}

void DeviceDX09::initDebugUI()
{
}

void DeviceDX09::WaitForPreviousFrame()
{
}

void DeviceDX09::prepareFrame()
{
}

void DeviceDX09::finishFrame()
{
}

size_t DeviceDX09::getFrameBufferCount() const
{
    return 0u;
}

const LOGLW::String& DeviceDX09::getName() const
{
    return m_name;
}

SDL2W::RenderTypes::RendererType DeviceDX09::getType() const
{
    return SDL2W::RenderTypes::RendererType::DIRECTX_9;
}

#endif  // GAME_ENGINE_WINDOWS
