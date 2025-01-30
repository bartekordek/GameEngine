#include "SDL2Wrapper/IWindow.hpp"
#include "SDL2Wrapper/WindowData.hpp"

#include "SDL2Wrapper/IMPORT_SDL.hpp"
#include "SDL2Wrapper/IMPORT_SDL_video.hpp"
#include "SDL2Wrapper/IMPORT_SDL_opengl.hpp"

#ifdef _MSC_VER
#include "SDL2Wrapper/IMPORT_DX9.hpp"
#endif

#include "CUL/Log/ILogContainer.hpp"

using namespace SDL2W;

IWindow::IWindow()
{
}

void IWindow::toggleDoubleBuffer( bool enable )
{
    SDL_bool sdlEnabled = enable ? SDL_TRUE : SDL_FALSE;
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, sdlEnabled );
}

void IWindow::setDepthSize( int value )
{
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, value );
}

void IWindow::setStencilSize( int value )
{
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, value );
}

void IWindow::setProfileMask( GLProfileMask value )
{
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, (int)value );
}

void IWindow::setContextFlag( GLContextFlag value )
{
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, (int) value );
}

void IWindow::setGLContextVersion( int major, int minor )
{
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, major );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, minor );
}

void* IWindow::createContext()
{
    return SDL_GL_CreateContext( getSDLWindow() );
}

void IWindow::destroyContext( void* context )
{
    SDL_GL_DeleteContext( context );  // This is basically void* !
}

SDL_Window* IWindow::getSDLWindow()
{
    return m_window;
}

void IWindow::setSize( uint16_t width, uint16_t height )
{
    WinSize winSize;
    winSize.setSize( width, height );
    setSize( winSize );
}

RenderTypes::RendererType IWindow::getCurrentRendererType() const
{
    return m_currentRenderer;
}

void IWindow::setCurrentRendererType( const RenderTypes::RendererType type )
{
    m_currentRenderer = type;
}

void IWindow::setWindowID( unsigned id )
{
    m_winId = id;
}

unsigned int IWindow::getWindowID() const
{
    return m_winId;
}

#if defined(SDL2W_WINDOWS)
IDirect3DDevice9* IWindow::createDX9Device()
{
#ifdef _MSC_VER
    //m_d9xDevice = SDL_RenderGetD3D9Device( m_renderer );
#endif // _MSC_VER
    return m_d9xDevice;
}

ID3D11Device* IWindow::createDX11Device()
{
#ifdef _MSC_VER
    //m_dx11Device = SDL_RenderGetD3D11Device( m_renderer );
#endif
    return m_dx11Device;
}

IDirect3DDevice9* IWindow::gertDX9Device() const
{
    return m_d9xDevice;
}

HWND IWindow::getHWND() const
{
    return m_hwnd;
}
#endif

IWindow::~IWindow()
{
}