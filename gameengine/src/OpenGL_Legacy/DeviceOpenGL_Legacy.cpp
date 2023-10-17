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

void DeviceOpenGL_Legacy::resetMatrixToIdentity( const MatrixTypes )
{
}

void DeviceOpenGL_Legacy::setViewport( const Viewport& )
{
}

void DeviceOpenGL_Legacy::setOrthogonalPerspective( const Camera& )
{
}

void DeviceOpenGL_Legacy::setPerspectiveProjection( const Camera& )
{
}

void DeviceOpenGL_Legacy::lookAt( const Camera& )
{
}

void DeviceOpenGL_Legacy::lookAt( const std::array<Pos3Dd, 3>& )
{
}

void DeviceOpenGL_Legacy::lookAt( const Pos3Dd&, const Pos3Dd&, const Pos3Dd& )
{
}

void DeviceOpenGL_Legacy::prepareFrame()
{
}

void DeviceOpenGL_Legacy::finishFrame()
{
}

std::uint32_t DeviceOpenGL_Legacy::createProgram( const CUL::String& name )
{
    return 0;
}

void DeviceOpenGL_Legacy::removeProgram( unsigned )
{
}

void DeviceOpenGL_Legacy::useProgram( int )
{
}

void DeviceOpenGL_Legacy::linkProgram( unsigned )
{
}

void DeviceOpenGL_Legacy::validateProgram( unsigned )
{
}

int DeviceOpenGL_Legacy::getCurrentProgram() const
{
    return 0;
}

unsigned int DeviceOpenGL_Legacy::createShader( const IFile& )
{
    return 0;
}

void DeviceOpenGL_Legacy::attachShader( unsigned, unsigned )
{
}

void DeviceOpenGL_Legacy::dettachShader( unsigned, unsigned )
{
}

void DeviceOpenGL_Legacy::removeShader( unsigned )
{
}

ContextInfo DeviceOpenGL_Legacy::initContextVersion( SDL2W::IWindow* )
{
    return ContextInfo();
}

void DeviceOpenGL_Legacy::setAttribValue( int, float )
{
}

void DeviceOpenGL_Legacy::setAttribValue( int, int )
{
}

void DeviceOpenGL_Legacy::setAttribValue( int, unsigned )
{
}

void DeviceOpenGL_Legacy::setAttribValue( int, bool )
{
}

void DeviceOpenGL_Legacy::setAttribValue( int , const CUL::String&  )
{
}

void DeviceOpenGL_Legacy::setUniformValue( int, float )
{
}

void DeviceOpenGL_Legacy::setUniformValue( int, int )
{
}

void DeviceOpenGL_Legacy::setUniformValue( int uniformLocation, unsigned value )
{
}

void DeviceOpenGL_Legacy::setUniformValue( int uniformLocation, const glm::vec2& val )
{
}

void DeviceOpenGL_Legacy::setUniformValue( int uniformLocation, const glm::vec3& val )
{
}

void DeviceOpenGL_Legacy::setUniformValue( int uniformLocation, const glm::vec4& val )
{
}

void DeviceOpenGL_Legacy::setUniformValue( int uniformLocation, const glm::mat2& val )
{
}

void DeviceOpenGL_Legacy::setUniformValue( int uniformLocation, const glm::mat3& val )
{
}

void DeviceOpenGL_Legacy::setUniformValue( int uniformLocation, const glm::mat4& val )
{
}

void DeviceOpenGL_Legacy::setProjectionAndModelToIdentity()
{
}

void DeviceOpenGL_Legacy::clearColorAndDepthBuffer()
{
}

void DeviceOpenGL_Legacy::createQuad( float scale )
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

void DeviceOpenGL_Legacy::bufferData( uint8_t bufferId, const CUL::MATH::Primitives::Quad& data, const BufferTypes type )
{
}

void DeviceOpenGL_Legacy::bufferData( uint8_t bufferId, const std::vector<unsigned int>& data, const BufferTypes type )
{
}

void DeviceOpenGL_Legacy::bufferData( uint8_t bufferId, const std::vector<float>& data, const BufferTypes type )
{
}

void DeviceOpenGL_Legacy::bufferData( uint8_t bufferId, const float vertices[], BufferTypes type )
{
}

void DeviceOpenGL_Legacy::bufferData( uint8_t bufferId, const std::vector<TextureData2D>& data, const BufferTypes type )
{
}

void DeviceOpenGL_Legacy::bufferSubdata( uint8_t bufferId, const BufferTypes type, std::vector<TextureData2D>& data )
{
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

unsigned int DeviceOpenGL_Legacy::generateElementArrayBuffer( const std::vector<unsigned int>& data, const int size )
{
    return 0;
}

unsigned int DeviceOpenGL_Legacy::generateAndBindBuffer( const BufferTypes bufferType, const int size )
{
    return 0;
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

void DeviceOpenGL_Legacy::drawElements( const PrimitiveType type, const std::vector<unsigned int>& data )
{
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

std::vector<std::string> DeviceOpenGL_Legacy::listExtensions()
{
    return std::vector<std::string>();
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

void DeviceOpenGL_Legacy::draw( const LineData& values, const ColorS& color )
{
}

void DeviceOpenGL_Legacy::draw( const TriangleData& values, const std::array<ColorS, 3>& color )
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

void DeviceOpenGL_Legacy::draw( const Point& , const ColorS& )
{
}

void DeviceOpenGL_Legacy::draw( const QuadCUL& , const glm::mat4& , const ColorS& )
{
}

void DeviceOpenGL_Legacy::draw( const TriangleCUL& , const glm::mat4& , const ColorS& )
{
}

void DeviceOpenGL_Legacy::translate( const Point& )
{
}

void DeviceOpenGL_Legacy::translate( const float , const float , const float )
{
}

void DeviceOpenGL_Legacy::rotate( const CUL::MATH::Rotation& )
{
}

void DeviceOpenGL_Legacy::rotate( const float , const float , const float , const float )
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

void DeviceOpenGL_Legacy::setTextureData( uint8_t, const TextureInfo& )
{
}

void DeviceOpenGL_Legacy::freeTexture( unsigned int& )
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

unsigned DeviceOpenGL_Legacy::getGPUCurrentAvailableMemoryKb()
{
    return 0;
}

const String& DeviceOpenGL_Legacy::getName() const
{
    return m_name;
}

SDL2W::RenderTypes::RendererType DeviceOpenGL_Legacy::getType() const
{
    return SDL2W::RenderTypes::RendererType::OPENGL_LEGACY;
}