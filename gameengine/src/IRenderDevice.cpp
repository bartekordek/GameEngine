#include "gameengine/IRenderDevice.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/Shaders/ShaderUnit.hpp"
#include "gameengine/Windowing/IWindow.hpp"

#include "RunOnRenderThread.hpp"
#include "CUL/CULInterface.hpp"
#include "CUL/Threading/ThreadUtil.hpp"

using namespace LOGLW;

static CUL::LOG::ILogger* g_logger = nullptr;

#if _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // #if _MSC_VER
const String TextureInfo::toString() const
{
    String result;

    result = "textureId: " + String( textureId );
    result += ", level: " + String( level );
    result += ", pixelFormat = " + String( (unsigned)pixelFormat );
    result += ", border = " + String( border );
    result += ", dataType = " + String( (int)dataType );
    result += ", data = " + String( (void*)data );
    result += ", size = " + size.toString();

    return result;
}
#if _MSC_VER
#pragma warning( pop )
#endif // #if _MSC_VER

IRenderDevice::IRenderDevice( bool forceLegacy )
    : m_forceLegacy( forceLegacy ), m_logger( CUL::CULInterface::getInstance()->getLogger() )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        //CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    g_logger = m_logger;
}

bool IRenderDevice::getIsEmbeddedSystems() const
{
    return m_isEmbeddedSystems;
}

std::vector<AttributeInfo> IRenderDevice::fetchProgramAttributeInfo( std::int32_t inProgramId ) const
{
    std::vector<AttributeInfo> result;
    throw std::logic_error( "Method not implemented" );

    return result;
}

std::vector<UniformInfo> IRenderDevice::fetchProgramUniformsInfo( std::int32_t inProgramId ) const
{
    std::vector<UniformInfo> result;
    throw std::logic_error( "Method not implemented" );

    return result;
}

void IRenderDevice::log( const String& text, const CUL::LOG::Severity severity ) const
{
    customAssert( m_logger != nullptr, "Logger utility is uninitialized inside of DeviceOpenGL." );

    if( m_lastLog != text )
    {
        m_logger->log( text, severity );
        m_lastLog = text;
    }
}

void IRenderDevice::customAssert( const bool value, const CUL::String& message ) const
{
    CUL::Assert::simple( value, message );
}

CUL::GUTILS::Version IRenderDevice::getVersion() const
{
    return m_supportedVersion;
}

void IRenderDevice::toggleDebugOutput( bool )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::checkLastCommandForErrors()
{
    throw std::logic_error( "Method not implemented" );
}

unsigned int IRenderDevice::generateAndBindBuffer( const BufferTypes /* bufferType*/, const int /* size = 1 */ )
{
    throw std::logic_error( "Method not implemented" );
    return 0u;
}

void IRenderDevice::setObjectName( EObjectType, std::uint32_t, const CUL::String& )
{
    throw std::logic_error( "Method not implemented." );
}

void IRenderDevice::removeShader( unsigned shaderId )
{
    throw std::logic_error( "Method not implemented." );
}

void IRenderDevice::setAttribValue( int /*attributeLocation*/, float /*value*/ )
{
    CUL::Assert::simple( false, "Method not implemented." );
}

void IRenderDevice::setAttribValue( int /*attributeLocation*/, int /*value*/ )
{
    CUL::Assert::simple( false, "Method not implemented." );
}

void IRenderDevice::setAttribValue( int /*attributeLocation*/, unsigned /*value*/ )
{
    CUL::Assert::simple( false, "Method not implemented." );
}

void IRenderDevice::setAttribValue( int /*attributeLocation*/, bool /*value*/ )
{
    CUL::Assert::simple( false, "Method not implemented." );
}

void IRenderDevice::setAttribValue( int /*attributeLocation*/, const CUL::String& /*value*/ )
{
    CUL::Assert::simple( false, "Method not implemented." );
}

void IRenderDevice::setUniformValue( int /*uniformLocation*/, float /*value*/ )
{
    CUL::Assert::simple( false, "Method not implemented." );
}

void IRenderDevice::setUniformValue( int /*uniformLocation*/, int /*value*/ )
{
    CUL::Assert::simple( false, "Method not implemented." );
}

void IRenderDevice::setUniformValue( int /*uniformLocation*/, unsigned /*value*/ )
{
    CUL::Assert::simple( false, "Method not implemented." );
}

unsigned int IRenderDevice::generateElementArrayBuffer( const std::vector<unsigned int>&/* data*/, const int /*size*/ /*= 1 */ )
{
    throw std::logic_error( "Method not implemented" );
    return 0u;
}

unsigned IRenderDevice::generateTexture()
{
    throw std::logic_error( "Method not implemented" );
    return 0u;
}

std::vector<std::string> IRenderDevice::listExtensions()
{
    m_logger->log( "[IRenderDevice::listExtensions] NOT IMPLEMENTED!" );
    std::vector<std::string> result;
    return result;
}

unsigned IRenderDevice::getGPUTotalAvailableMemoryKb()
{
    throw std::logic_error( "Method not implemented" );
    return 0u;
}

unsigned IRenderDevice::getGPUCurrentAvailableMemoryKb()
{
    throw std::logic_error( "Method not implemented" );
    return 0u;
}

ShaderUnit* IRenderDevice::createShaderUnit( const CUL::FS::Path&, bool, CUL::String& )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::deleteShaderUnit( ShaderUnit* )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::bufferSubdata( BufferDataId, const BufferTypes, std::vector<TextureData2D>& )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::bufferData( BufferDataId, const std::vector<TextureData2D>&, const BufferTypes )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::bufferData( BufferDataId, const float[], BufferTypes )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::bufferData( BufferDataId, const std::vector<float>&, const BufferTypes )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::bufferData( BufferDataId, const CUL::DataWrapper& , const BufferTypes )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::bufferData( BufferDataId, const CUL::MATH::Primitives::Quad&, const BufferTypes )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::prepareFrame()
{
    throw std::logic_error( "Method not implemented" );
}

std::uint32_t IRenderDevice::createProgram( const CUL::String& name )
{
    throw std::logic_error( "Method not implemented" );
    return 0u;
}

void IRenderDevice::removeProgram( unsigned )
{
    throw std::logic_error("Method not implemented");
}

void IRenderDevice::useProgram( int programId )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::linkProgram( unsigned programId )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::validateProgram( std::uint32_t programId )
{
    throw std::logic_error("Method not implemented");
}

int IRenderDevice::getCurrentProgram() const
{
    throw std::logic_error( "Method not implemented" );
    return 0;
}

bool IRenderDevice::attachShader( unsigned programId, unsigned shaderId )
{
    throw std::logic_error( "Method not implemented" );
    return false;
}

void IRenderDevice::dettachShader( unsigned programId, unsigned shaderId )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::resetMatrixToIdentity( const MatrixTypes matrix )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::setViewport( const Viewport& )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::setOrthogonalPerspective( const Camera& )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::setPerspectiveProjection( const Camera& )
{
    throw std::logic_error( "Method not implemented" );
}

void IRenderDevice::lookAt(const Pos3Dd& eye, const Pos3Dd& center, const Pos3Dd& up)
{
    throw std::logic_error("Method not implemented");
}

void IRenderDevice::lookAt(const std::array<Pos3Dd, 3>& lookAtVec)
{
    throw std::logic_error("Method not implemented");
}

void IRenderDevice::lookAt(const Camera& vp)
{
    throw std::logic_error("Method not implemented");
}

void IRenderDevice::setTextureData(std::uint32_t, const TextureInfo& )
{
    throw std::logic_error("Method not implemented");
}

void IRenderDevice::freeTexture(std::uint32_t textureId)
{
    throw std::logic_error("Method not implemented");
}

void IRenderDevice::drawElements( const PrimitiveType, const CUL::DataWrapper& )
{
    CUL::Assert::check( false, "IRenderDevice::drawElements - Method not implemented." );
}

LOGLW::UniformValue IRenderDevice::getUniformValue( std::int32_t, std::int32_t, DataType )
{
    CUL::Assert::check( false, "IRenderDevice::getUniformValue - Method not implemented." );
    return LOGLW::UniformValue();
}

void IRenderDevice::createQuad( float scale )
{
    CUL::Assert::check( false, "IRenderDevice::createQuad - Method not implemented." );
}

IRenderDevice::~IRenderDevice()
{
}

