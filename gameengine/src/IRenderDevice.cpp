#include "gameengine/IRenderDevice.hpp"
#include "gameengine/Camera.hpp"

#include "SDL2Wrapper/IWindow.hpp"

#include "CUL/CULInterface.hpp"
#include "CUL/Threading/ThreadUtils.hpp"

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
    if( !CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        //CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    g_logger = m_logger;
}

bool IRenderDevice::getIsEmbeddedSystems() const
{
    return m_isEmbeddedSystems;
}


void IRenderDevice::log( const String& text, const CUL::LOG::Severity severity ) const
{
    customAssert( m_logger != nullptr, "Logger utility is unninitialized inside of DeviceOpenGL." );

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

IRenderDevice::~IRenderDevice()
{
}