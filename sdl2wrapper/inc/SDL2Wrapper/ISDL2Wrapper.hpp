#pragma once

#include "SDL2Wrapper/IEventLoop.hpp"
#include "SDL2Wrapper/IWindowEventObservable.hpp"
#include "SDL2Wrapper/Input/IKeyboardObservable.hpp"
#include "SDL2Wrapper/Input/IMouseObservable.hpp"
#include "SDL2Wrapper/RendererTypes.hpp"

#include "CUL/GenericUtils/IConfigFile.hpp"
#include "CUL/Graphics/ITexture.hpp"
#include "CUL/String.hpp"

#include "CUL/STL_IMPORTS/STD_map.hpp"
#include "CUL/STL_IMPORTS/STD_functional.hpp"

NAMESPACE_BEGIN( CUL )


class CULInterface;

NAMESPACE_BEGIN( FS )
class Path;
NAMESPACE_END( FS )

NAMESPACE_BEGIN( Graphics )
class ITexture;
NAMESPACE_END( Graphics )

NAMESPACE_BEGIN( LOG )
class ILogger;
NAMESPACE_END( LOG )

NAMESPACE_END( CUL )


NAMESPACE_BEGIN( SDL2W )

class ISDLEventObserver;
class IWindowEventObserver;
class ISprite;
class IWindow;
struct WindowData;

using WindowEventType = WindowEvent::Type;

using InitCallback = std::function<void()>;
using IConfigFile = CUL::GUTILS::IConfigFile;

class SDL2WAPI ISDL2Wrapper:
    public IKeyboardObservable,
    public IMouseObservable,
    public IWindowEventObservable,
    public IEventLoop
{
public:
    ISDL2Wrapper();

    virtual void init(
        const WindowData& wd,
        const CUL::FS::Path& configPath = "" ) = 0;
    virtual void refreshScreen() = 0;
    virtual void renderFrame( bool clearContext = true, bool refreshWindow = true ) = 0;
    virtual void clearWindows() = 0;
    int getRendererId( const RenderTypes::RendererType type ) const;
    const std::map<RenderTypes::RendererType, int>& getRenderersList() const;
    virtual void printAvailableRenderers() const = 0;

    virtual CUL::Graphics::ITexture* createTexture( const CUL::FS::Path& path, IWindow* targetWindow ) const = 0;
    virtual ISprite* createSprite( const CUL::FS::Path& path, IWindow* targetWindow ) const = 0;

    virtual ISprite* createSprite( CUL::Graphics::ITexture* tex, IWindow* targetWindow ) const = 0;

    virtual IWindow* getMainWindow() const = 0;

    virtual unsigned int getInputLatency() const = 0;
    virtual void setInputLatency( unsigned latencyInUs ) = 0;

    virtual void registerWindowEventListener( IWindowEventObserver* observer ) = 0;
    virtual void unregisterWindowEventListener( IWindowEventObserver* observer ) = 0;

    virtual void registerOnInitCallback( const InitCallback& callback ) = 0;
    virtual void registerSDLEventObserver( ISDLEventObserver* eventObserver )  = 0;
    virtual void unRegisterSDLEventObserver( ISDLEventObserver* eventObserver )  = 0;

    virtual KeyboardState& getKeyStates() = 0;

    virtual IConfigFile* getConfig() const = 0;

    virtual CUL::LOG::ILogger* getLogger() const = 0;
    virtual CUL::CULInterface* getCul() = 0;


    static ISDL2Wrapper* createSDL2Wrapper();

    virtual ~ISDL2Wrapper();
protected:
    std::map<RenderTypes::RendererType, int> m_renderers;

private:

private: // Deleted
    ISDL2Wrapper( const ISDL2Wrapper& rhv ) = delete;
    ISDL2Wrapper( ISDL2Wrapper&& rhv ) = delete;
    ISDL2Wrapper& operator=( const ISDL2Wrapper& rhv ) = delete;
    ISDL2Wrapper& operator=( ISDL2Wrapper&& rhv ) = delete;
};

NAMESPACE_END( SDL2W )