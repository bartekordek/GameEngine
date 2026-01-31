#pragma once

#include "gameengine/Render/IRender.hpp"
#include "gameengine/Render/RendererTypes.hpp"
#include "gameengine/String.hpp"
#include "gameengine/IMPORT_Windows.hpp"

#include "CUL/Graphics/IObjectRegister.hpp"
#include "CUL/IName.hpp"
#include "CUL/Graphics/Size2D.hpp"
#include "CUL/Graphics/Pos2D.hpp"

#include "CUL/STL_IMPORTS/STD_map.hpp"

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

NAMESPACE_BEGIN( LOGLW )

using CDbl = const double;
using CShrt = const unsigned short;
using SmallCount = CShrt;

#ifdef _MSC_VER
class GAME_ENGINE_API CUL::IName;
#endif
class ISDL2Wrapper;
class ISprite;
class WinData;
struct WinPos;
struct WinSize;

using Vector3Di = CUL::MATH::Vector3Di;
using Vector3Du = CUL::MATH::Vector3Du;
using ColorS = CUL::Graphics::ColorS;
using ColorE = CUL::Graphics::ColorE;
using IName = CUL::IName;

using TextureMap = std::map<String, std::unique_ptr<CUL::Graphics::ITexture>>;

#ifdef _MSC_VER
    #pragma warning( push )
    #pragma warning( disable : 4820 )
#endif

enum class GLProfileMask : short
{
    NONE = -1,
    CORE = 0x0001,           // SDL_GL_CONTEXT_PROFILE_CORE
    COMPATIBILITY = 0x0002,  // SDL_GL_CONTEXT_PROFILE_COMPATIBILITY
    ES = 0x0004              // SDL_GL_CONTEXT_PROFILE_ES
};

enum class GLContextFlag
{
    DEBUG_FLAG = 0x0001,
    FORWARD_COMPATIBLE_FLAG = 0x0002,
    ROBUST_ACCESS_FLAG = 0x0004,
    RESET_ISOLATION_FLAG = 0x0008
};

class GAME_ENGINE_API IWindow: public IRender, public IName, public CUL::Graphics::IObjectRegister
{
public:
    enum class Type : char
    {
        NONE = 0,
        SDL_WIN,
        SDL_OPENGL
    };

    IWindow();
    IWindow( const WinData& data ) = delete;

    virtual ~IWindow();

    virtual const WinPos& getPos() const = 0;
    virtual void setPos( const WinPos& pos ) = 0;

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

    SDL_Window* getSDLWindow();

    void setSize( uint16_t width, uint16_t height );

    RenderTypes::RendererType getCurrentRendererType() const;
    void setCurrentRendererType( const RenderTypes::RendererType type );

    virtual operator ::SDL_Window*() = 0;
    virtual operator const ::SDL_Window*() = 0;

#if defined( CUL_WINDOWS )
    IDirect3DDevice9* createDX9Device();
    ID3D11Device* createDX11Device();
    IDirect3DDevice9* gertDX9Device() const;

    HWND getHWND() const;
#endif

protected:
    SDL_Window* m_window = nullptr;

#if defined( CUL_WINDOWS )
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

NAMESPACE_END( LOGLW )