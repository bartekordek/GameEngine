#pragma once

#include "SDL2Wrapper/IRender.hpp"
#include "SDL2Wrapper/RendererTypes.hpp"

#include "CUL/Graphics/IObjectRegister.hpp"

#include "CUL/IName.hpp"
#include "CUL/Graphics/Size2D.hpp"
#include "CUL/Graphics/Pos2D.hpp"

#include "CUL/STL_IMPORTS/STD_map.hpp"

#include "SDL2Wrapper/IMPORT_Windows.hpp"

struct SDL_Window;
struct IDirect3DDevice9;
struct ID3D11Device;

NAMESPACE_BEGIN( CUL )
NAMESPACE_BEGIN( Graphics )
class ITexture;
NAMESPACE_END( Graphics )

NAMESPACE_BEGIN( Video )
class FPSCounter;
NAMESPACE_END( Video )

NAMESPACE_BEGIN( FS )
class Path;
NAMESPACE_END( FS )

NAMESPACE_END( CUL )


NAMESPACE_BEGIN( SDL2W )

using CDbl = const double;
using CShrt = const unsigned short;
using SmallCount = CShrt;
using Size2Du = CUL::Graphics::Size2Du;
using Size2Di = CUL::Graphics::Size2Di;

#ifdef _MSC_VER
class SDL2WAPI CUL::IName;
#endif
class ISDL2Wrapper;
class ISprite;
struct WinSize;

using Vector3Di = CUL::MATH::Vector3Di;
using Vector3Du = CUL::MATH::Vector3Du;
using ColorS = CUL::Graphics::ColorS;
using ColorE = CUL::Graphics::ColorE;
using IName = CUL::IName;
using String = CUL::String;

using TextureMap = std::map<CUL::String, std::unique_ptr<CUL::Graphics::ITexture>>;

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable: 4820 )
#endif

enum class GLProfileMask: short
{
    NONE = -1,
    CORE =  0x0001, // SDL_GL_CONTEXT_PROFILE_CORE
    COMPATIBILITY  = 0x0002, // SDL_GL_CONTEXT_PROFILE_COMPATIBILITY
    ES             = 0x0004 // SDL_GL_CONTEXT_PROFILE_ES
};

enum class GLContextFlag
{
    DEBUG_FLAG              = 0x0001,
    FORWARD_COMPATIBLE_FLAG = 0x0002,
    ROBUST_ACCESS_FLAG      = 0x0004,
    RESET_ISOLATION_FLAG    = 0x0008
};

class SDL2WAPI IWindow:
    public IRender,
    public IName,
    public CUL::Graphics::IObjectRegister
{
public:
    enum class Type: char
    {
        NONE = 0,
        SDL_WIN,
        SDL_OPENGL
    };

    IWindow();
    IWindow( const struct WindowData& data ) = delete;

    virtual ~IWindow();

    virtual const CUL::Graphics::Pos2Di& getPos() const = 0;
    virtual void setPos( const CUL::Graphics::Pos2Di& pos ) = 0;

    virtual const WinSize& getSize() const = 0;
    virtual const WinSize& getCurrentScreenNativeResolution() const = 0;
    virtual void setSize( const WinSize& size ) = 0;

    virtual Type getType() const = 0;

    virtual CUL::Graphics::ITexture* createTexture( const CUL::FS::Path& path ) = 0;
    virtual ISprite* createSprite( const CUL::FS::Path& path ) = 0;
    virtual ISprite* createSprite( CUL::Graphics::ITexture* tex ) = 0;

    void setWindowID( unsigned id );
    unsigned int getWindowID() const;

    void toggleDoubleBuffer( bool enable );
    void setDepthSize( int value );
    void setStencilSize( int value );
    void setGLContextVersion( int major, int minor );
    void setProfileMask( GLProfileMask value );
    void setContextFlag( GLContextFlag value );

    void* createContext();
    void destroyContext( void* context );

    virtual void setFullscreen( bool fullscreen ) = 0;

    virtual void toggleFpsCounter( bool on, short unsigned everyNsecond = 2 ) = 0;

    virtual CUL::Video::FPSCounter* getFpsCounter() = 0;

    SDL_Window* getSDLWindow();

    void setSize( uint16_t width, uint16_t height );

    RenderTypes::RendererType getCurrentRendererType() const;
    void setCurrentRendererType( const RenderTypes::RendererType type );

    virtual operator ::SDL_Window*( ) = 0;
    virtual operator const ::SDL_Window*( ) = 0;

#if defined(SDL2W_WINDOWS)
    IDirect3DDevice9* createDX9Device();
    ID3D11Device* createDX11Device();
    IDirect3DDevice9* gertDX9Device() const;

    HWND getHWND() const;
#endif

protected:
    SDL_Window* m_window = nullptr;

#if defined(SDL2W_WINDOWS)
    IDirect3DDevice9* m_d9xDevice = nullptr;
    ID3D11Device* m_dx11Device = nullptr;
    HWND m_hwnd;
#endif

private:
    RenderTypes::RendererType m_currentRenderer{ RenderTypes::RendererType::NONE };
    String m_rendererName;
    unsigned int m_winId = 0;

    IWindow( const IWindow& wind ) = delete;
    IWindow( IWindow&& wind ) = delete;
    IWindow& operator=( const IWindow& window ) = delete;
    IWindow& operator=( IWindow&& window ) = delete;
};

#ifdef _MSC_VER
#pragma warning( pop )
#endif

NAMESPACE_END( SDL2W )