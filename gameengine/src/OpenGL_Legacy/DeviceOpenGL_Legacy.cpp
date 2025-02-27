#include "OpenGL_Legacy/DeviceOpenGL_Legacy.hpp"

using namespace LOGLW;

void* DeviceOpenGL_Legacy::getNativeDevice()
{
    return nullptr;
}

bool DeviceOpenGL_Legacy::isLegacy()
{
    return false;
}

size_t DeviceOpenGL_Legacy::getFrameBufferCount() const
{
    return size_t();
}

void DeviceOpenGL_Legacy::initDebugUI()
{
}

void DeviceOpenGL_Legacy::prepareFrame()
{
}

void DeviceOpenGL_Legacy::finishFrame()
{
}

void DeviceOpenGL_Legacy::useProgram( int )
{
}

ContextInfo DeviceOpenGL_Legacy::initContextVersion( LOGLW::IWindow* )
{
    return ContextInfo();
}

void DeviceOpenGL_Legacy::setProjectionAndModelToIdentity()
{
}

void DeviceOpenGL_Legacy::clearColorAndDepthBuffer()
{
}

void DeviceOpenGL_Legacy::clearColorTo( const ColorS color )
{
}

void DeviceOpenGL_Legacy::clearBuffer( const ClearMasks mask )
{
}

unsigned int DeviceOpenGL_Legacy::generateVertexArray( const int size )
{
    return 0;
}

void DeviceOpenGL_Legacy::setClientState( ClientStateTypes cs, bool enabled )
{
}

void DeviceOpenGL_Legacy::texCoordPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer )
{
}

void DeviceOpenGL_Legacy::vertexPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer )
{
}

void DeviceOpenGL_Legacy::setVertexArrayClientState( const bool enable )
{
}

void DeviceOpenGL_Legacy::setColorClientState( bool enable )
{
}

void DeviceOpenGL_Legacy::deleteBuffer( BufferTypes bufferType, unsigned& id )
{
}

void DeviceOpenGL_Legacy::enableVertexAttribiute( unsigned programId, const String& attribName )
{
}

void DeviceOpenGL_Legacy::disableVertexAttribiute( unsigned programId, const String& attribName )
{
}

int DeviceOpenGL_Legacy::getAttribLocation( unsigned programId, const String& attribName )
{
    return 0;
}

int DeviceOpenGL_Legacy::getUniformLocation( unsigned programId, const String& attribName )
{
    return 0;
}

void DeviceOpenGL_Legacy::unbindBuffer( const BufferTypes bufferType )
{
}

void DeviceOpenGL_Legacy::bindBuffer( const BufferTypes bufferType, unsigned bufferId )
{
}

unsigned int DeviceOpenGL_Legacy::generateBuffer( const BufferTypes type, const int size )
{
    return 0;
}

void DeviceOpenGL_Legacy::drawElements( const PrimitiveType type, const std::vector<float>& data )
{
}

void DeviceOpenGL_Legacy::drawElementsFromLastBuffer( const PrimitiveType primitiveType, const DataType dataType, unsigned count )
{
}

void DeviceOpenGL_Legacy::drawArrays( unsigned vaoId, const PrimitiveType primitiveType, unsigned first, unsigned count )
{
}

void DeviceOpenGL_Legacy::vertexAttribPointer( const VertexData& )
{
}

void DeviceOpenGL_Legacy::enableVertexAttribArray( unsigned attributeId )
{
}

void DeviceOpenGL_Legacy::setVertexPointer( int coordinatesPerVertex, DataType dataType, int stride, const void* data )
{
}

void DeviceOpenGL_Legacy::draw( const QuadCUL& quad, const QuadCUL& texQuad )
{
}

void DeviceOpenGL_Legacy::draw( const QuadCUL& quad, const ColorS& color )
{
}

void DeviceOpenGL_Legacy::draw( const QuadCUL& quad, const Point& translation, const CUL::MATH::Rotation& rotation, const ColorS& color )
{
}

void DeviceOpenGL_Legacy::draw( const QuadCUL& quad, const QuadColors& color )
{
}

void DeviceOpenGL_Legacy::draw( const QuadData& quad, const std::array<ColorS, 4>& color )
{
}

void DeviceOpenGL_Legacy::draw( const CUL::MATH::Primitives::Line& values, const ColorS& color )
{
}

void DeviceOpenGL_Legacy::draw( const LineData& values, const LineColors& color )
{
}

void DeviceOpenGL_Legacy::draw( const LineData& /*values*/, const ColorS& /*color*/ )
{
}

void DeviceOpenGL_Legacy::draw( const TriangleData& /*values*/, const std::array<ColorS, 3>& /*color*/ )
{
}

void DeviceOpenGL_Legacy::draw( const TriangleCUL& /*quad*/, const std::array<ColorS, 4>& /*color*/ )
{
}

void DeviceOpenGL_Legacy::draw( const TriangleCUL& /*triangle*/, const ColorS& /*color*/ )
{
}

void DeviceOpenGL_Legacy::draw( const QuadData& /*quad*/, const ColorS& /*color*/ )
{
}

void DeviceOpenGL_Legacy::draw( const Point&, const ColorS& )
{
}

void DeviceOpenGL_Legacy::draw( const QuadCUL&, const glm::mat4&, const ColorS& )
{
}

void DeviceOpenGL_Legacy::draw( const TriangleCUL&, const glm::mat4&, const ColorS& )
{
}

void DeviceOpenGL_Legacy::translate( const Point& )
{
}

void DeviceOpenGL_Legacy::translate( const float, const float, const float )
{
}

void DeviceOpenGL_Legacy::rotate( const CUL::MATH::Rotation& )
{
}

void DeviceOpenGL_Legacy::rotate( const float, const float, const float, const float )
{
}

void DeviceOpenGL_Legacy::scale( const CUL::MATH::Vector3Df& )
{
}

void DeviceOpenGL_Legacy::scale( const float )
{
}

void DeviceOpenGL_Legacy::setDepthTest( const bool )
{
}

void DeviceOpenGL_Legacy::setBackfaceCUll( const bool )
{
}

void DeviceOpenGL_Legacy::setTexuring( const bool )
{
}

unsigned DeviceOpenGL_Legacy::generateTexture()
{
    return 0;
}

void DeviceOpenGL_Legacy::setActiveTextureUnit( ETextureUnitIndex )
{
}

void DeviceOpenGL_Legacy::bindTexture( const unsigned int )
{
}

void DeviceOpenGL_Legacy::setTextureParameter( uint8_t, const TextureParameters, const TextureFilterType )
{
}

void DeviceOpenGL_Legacy::matrixStackPush()
{
}

void DeviceOpenGL_Legacy::matrixStackPop()
{
}

unsigned DeviceOpenGL_Legacy::getGPUTotalAvailableMemoryKb()
{
    return 0;
}

const String& DeviceOpenGL_Legacy::getName() const
{
    return m_name;
}

LOGLW::RenderTypes::RendererType DeviceOpenGL_Legacy::getType() const
{
    return LOGLW::RenderTypes::RendererType::OPENGL_LEGACY;
}
