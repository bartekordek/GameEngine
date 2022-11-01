#include "DeviceOpenGL_Legacy.hpp"

#include "CUL/CULInterface.hpp"
#include "CUL/Threading/ThreadUtils.hpp"

#include "IMPORT_glew.hpp"
#include "ImportFreeglut.hpp"

using namespace LOGLW;

static CUL::LOG::ILogger* g_loggerOGL_Legacy = nullptr;

DeviceOpenGL_Legacy::DeviceOpenGL_Legacy( CUL::CULInterface* culInterface): IRenderDevice( culInterface )
{
    g_loggerOGL_Legacy = culInterface->getLogger();

    m_forceLegacy = true;

    log( "DeviceOpenGL_Legacy::DeviceOpenGL_Legacy();" );

    glGetIntegerv( GL_MAJOR_VERSION, &m_supportedVersion.major );
    checkLastCommandForErrors();

    glGetIntegerv( GL_MINOR_VERSION, &m_supportedVersion.minor );
    checkLastCommandForErrors();
}

ContextInfo DeviceOpenGL_Legacy::initContextVersion( SDL2W::IWindow* window )
{
    if( !getCUL()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        getCUL()->getThreadUtils().setCurrentThreadName( "RenderThread" );
        if( !getCUL()->getThreadUtils().getIsCurrentThreadNameEqualTo("RenderThread") )
        {
            CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
        }
    }
}

void DeviceOpenGL_Legacy::checkLastCommandForErrors()
{
    const GLenum err = glGetError();
    const GLubyte* errorAsString = gluErrorString( err );
    customAssert( GL_NO_ERROR == err, "Error creating program, error numer: " + CUL::String( errorAsString ) );
}