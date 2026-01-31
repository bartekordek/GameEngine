#include "SDL2WrapperImpl.hpp"
#include "gameengine/Input/IKeyboardObserver.hpp"
#include "gameengine/Input/IMouseObserver.hpp"
#include "gameengine/Events/IWindowEventListener.hpp"

#include "TextureSDL.hpp"

#include "sdl2wrapper/IMPORT_SDL.hpp"

#include "CUL/ITimer.hpp"
#include "CUL/Threading/ThreadUtil.hpp"

#ifdef _MSC_VER
    #pragma warning( push, 0 )
    #pragma warning( disable : 4710 )
#endif

using namespace LOGLW;

constexpr bool SDLBoolToCppBool( const SDL_bool value )
{
    return value == 1 ? true : false;
}

SDL2WrapperImpl::SDL2WrapperImpl()
{
}

#ifdef _MSC_VER
    // Yes, I know that is a Spectre mitigation.
    // But for now, I let this as TODO, since i don't know
    // How to fix this.
    // TODO
    #pragma warning( push )
    #pragma warning( disable : 5045 )
#endif

void SDL2WrapperImpl::init( const WinData& wd, const CUL::FS::Path& configPath )
{
    m_windowData = wd;

    m_culInterface = CUL::CULInterface::createInstance( configPath );
    m_logger = m_culInterface->getLogger();

    m_configFile = m_culInterface->getConfig();

    m_logger->log( "Initializing SDL..." );
    const auto sdlInitSuccess = SDL_Init( SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS );
    if( 0 != sdlInitSuccess )
    {
        CUL::Assert::simple( false, SDL_GetError() );
    }
    m_logger->log( "Initializing SDL... Done." );

    const std::int32_t cpuCount = SDL_GetCPUCount();
    m_logger->logVariable( CUL::LOG::Severity::Info, "Available cpu cores: %d", cpuCount );

    const auto cpuCacheSize = SDL_GetCPUCacheLineSize();
    m_logger->logVariable( CUL::LOG::Severity::Info, "Available cache size: %d", cpuCacheSize );

    const auto hasRDTSC = SDLBoolToCppBool( SDL_HasRDTSC() );
    m_logger->logVariable( CUL::LOG::Severity::Info, "CPU has the RDTSC instruction: %s", hasRDTSC ? "true" : "false" );

    const size_t renderDriversCount = fetchRenderTypes();
    m_logger->logVariable( CUL::LOG::Severity::Info, "Available render drivers count: %d\n", renderDriversCount );

    m_logger->log( "#################################################################################" );
    m_logger->log( "#################################################################################" );
    m_logger->log( "Renderers:" );

    if( m_windowData.RendererType == RenderTypes::RendererType::NONE )
    {
        const auto rendererName = m_configFile ? m_configFile->getValue( "RENDERER" ) : "";
        if( rendererName.empty() )
        {
            m_windowData.RendererType = RenderTypes::RendererType::OPENGL_MODERN;
        }
        else
        {
            m_windowData.RendererType = RenderTypes::convertToEnum( rendererName.getString() );
        }
    }

    m_windowFactory = new WindowCreatorConcrete( m_logger );
    m_mainWindow = dynamic_cast<RegularSDL2Window*>( m_windowFactory->createWindow( m_windowData, this ) );
    m_windows[m_mainWindow->getWindowID()] = std::unique_ptr<IWindow>( m_mainWindow );

    if( ( m_windowData.RendererType == RenderTypes::RendererType::DIRECTX_9 ) ||
        ( m_windowData.RendererType == RenderTypes::RendererType::DIRECTX_11 ) )
    {
        Uint32 flags = 0u;
        m_renderer = SDL_CreateRenderer( m_mainWindow->getSDLWindow(), m_renderers[m_windowData.RendererType], flags );
    }

    createKeys();

    for( auto i = static_cast<MouseButtonIndex>( SDL_BUTTON_LEFT ); i <= static_cast<MouseButtonIndex>( SDL_BUTTON_X2 ); ++i )
    {
        m_mouseData.setState( i, true );
    }

    if( m_onInitCallback )
    {
        m_onInitCallback();
    }
}

size_t SDL2WrapperImpl::fetchRenderTypes()
{
    size_t renderDriversCount = SDL_GetNumRenderDrivers();

    SDL_RendererInfo renderInfo;
    for( size_t i = 0; i < renderDriversCount; ++i )
    {
        m_logger->log( "#################################################################################" );
        CUL::Assert::check( SDL_GetRenderDriverInfo( i, &renderInfo ) == 0, "Cannnot get driver info for index = %d", i );
        String rendererName( renderInfo.name );
        m_logger->logVariable( CUL::LOG::Severity::Info, "Renderer name: %s", rendererName.getUtfChar() );
        m_logger->logVariable( CUL::LOG::Severity::Info, "Max texture size: %d, %d", renderInfo.max_texture_width,
                               renderInfo.max_texture_height );
        m_logger->log( "Available texture formats: " );
        for( Uint32 iTexFormat = 0; iTexFormat < renderInfo.num_texture_formats; ++iTexFormat )
        {
            m_logger->logVariable( CUL::LOG::Severity::Info, "%s", SDL_GetPixelFormatName( renderInfo.texture_formats[iTexFormat] ) );
        }

        m_logger->log( "#################################################################################\n" );

        m_renderers[RenderTypes::convertToEnum( renderInfo.name )] = i;
    }

    m_renderers[RenderTypes::RendererType::OPENGL_LEGACY] = 2;
    m_renderers[RenderTypes::RendererType::OPENGL_MODERN] = 2;

#if defined( CUL_WINDOWS )
    m_renderers[RenderTypes::RendererType::DIRECTX_12] = renderDriversCount;
    ++renderDriversCount;
#endif  // CUL_WINDOWS

    return renderDriversCount;
}

CUL::CULInterface* SDL2WrapperImpl::getCul()
{
    return m_culInterface.get();
}

IConfigFile* SDL2WrapperImpl::getConfig() const
{
    return m_configFile;
}

void SDL2WrapperImpl::createKeys()
{
    auto isKeyDown = []( const Uint8* sdlKey, size_t index )
    {
        SDL_Scancode scanCode = static_cast<SDL_Scancode>( index );
        return 0 != sdlKey[scanCode];
    };

    auto getKeyName = []( size_t index )
    {
        SDL_Scancode scanCode = static_cast<SDL_Scancode>( index );
        String result = SDL_GetScancodeName( scanCode );
        return result;
    };  

    m_logger->log( "SDL2WrapperImpl::createKeys()::Begin" );
    size_t keyCount = 0;
    auto kbrdState = SDL_GetKeyboardState( (int*)&keyCount );
    // Basically keyCount is the same as SDL_NUM_SCANCODES.
    for( size_t i = 0; i < keyCount; ++i )
    {
        const auto keyName = getKeyName( i );
        if( !keyName.empty() )
        {
            m_keys[keyName] = isKeyDown( kbrdState, i );
            const char* keyNameChar = keyName.getUtfChar();
            const String txt = String::createFromPrintf( "Name: %s, id: %d", keyNameChar, i );
            m_logger->logInfo( txt.getUtfChar() );
        }
    }
    m_logger->log( "SDL2WrapperImpl::createKeys()::End" );
}
#ifdef _MSC_VER
    #pragma warning( pop )
#endif

CUL::LOG::ILogger* SDL2WrapperImpl::getLogger() const
{
    return m_logger;
}

void SDL2WrapperImpl::renderFrame( bool clearContext, bool refreshWindow )
{
    if( true == clearContext )
    {
        clearWindows();
    }

    for( auto& window : m_windows )
    {
        window.second->renderAll();
    }

    if( true == refreshWindow )
    {
        refreshScreen();
    }
}

void SDL2WrapperImpl::clearWindows()
{
    for( auto& window : m_windows )
    {
        window.second->clearBuffers();
    }
}

void SDL2WrapperImpl::printAvailableRenderers() const
{
    m_logger->log( "SDL2WrapperImpl::printAvailableRenderers():" );
    for( const auto& renderer : m_renderers )
    {
        m_logger->logInfo( "Name: %s, id: %d", RenderTypes::convertToString( renderer.first ).getUtfChar(), renderer.second );
    }
}

void SDL2WrapperImpl::refreshScreen()
{
    for( auto& window : m_windows )
    {
        window.second->updateScreenBuffers();
    }
}

#include "CUL/STL_IMPORTS/STD_iostream.hpp"
void SDL2WrapperImpl::runEventLoop()
{
    m_culInterface->getThreadUtils().setThreadName( "SDL2WrapperImpl::runEventLoop()/main" );
    m_logger->log( "SDL2WrapperImpl::runEventLoop()::Begin" );

    while( eventLoopActive )
    {
        pollEvents();
        CUL::ITimer::sleepMicroSeconds( m_eventLatencyUs );
    }
    m_logger->log( "SDL2WrapperImpl::runEventLoop()::End" );
}

void SDL2WrapperImpl::pollEvents()
{
    static SDL_Event event;
    if( SDL_PollEvent( &event ) > 0 )
    {
        handleEvent( event );
        notifySDLEventObservers( event );
    }
}

void SDL2WrapperImpl::notifySDLEventObservers( SDL_Event& event )
{
    std::lock_guard<std::mutex> lock( m_sdlEventObserversMtx );
    for( auto eventObserver : m_sdlEventObservers )
    {
        eventObserver->handleEvent( event );
    }
}

void SDL2WrapperImpl::handleEvent( const SDL_Event& event )
{
    if( ( event.type == SDL_KEYDOWN || event.type == SDL_KEYUP ) )
    {
        if( SDL_SCANCODE_UNKNOWN != event.key.keysym.scancode )
        {
            handleKeyboardEvent( event );
        }
    }

    if( isMouseEvent( event ) )
    {
        handleMouseEvent( event );
        notifyMouseListerners( m_mouseData );
        notifyMouseCallbacks( m_mouseData );
    }

    if( isWindowEvent( event ) )
    {
        handleWindowEvent( event );
    }
}

bool SDL2WrapperImpl::isMouseEvent( const SDL_Event& event )
{
    return event.type >= SDL_MOUSEMOTION && event.type <= SDL_MOUSEWHEEL;
}

bool SDL2WrapperImpl::isWindowEvent( const SDL_Event& event )
{
    switch( event.type )
    {
        case SDL_QUIT:
        case SDL_APP_TERMINATING:
        case SDL_WINDOWEVENT:
        case SDL_WINDOWEVENT_MOVED:
        case SDL_WINDOWEVENT_ENTER:
        case SDL_WINDOWEVENT_LEAVE:
            return true;
        default:
            return false;
            break;
    }
}

void SDL2WrapperImpl::handleKeyboardEvent( const SDL_Event& sdlEvent )
{
    const bool keyIsDown = ( SDL_KEYDOWN == sdlEvent.type ) ? true : false;
    auto& key = m_keys.at( SDL_GetScancodeName( sdlEvent.key.keysym.scancode ) );
    key = keyIsDown;

    notifyKeyboardCallbacks( m_keys );
    notifyKeyboardListeners( m_keys );
}

void SDL2WrapperImpl::handleMouseEvent( const SDL_Event& event )
{
    m_mouseData.setType( (MouseData::EventType)event.type );
    if( event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP )
    {
        const bool isUp = event.type == SDL_MOUSEBUTTONUP ? true : false;

        const auto mb = event.button;
        const auto mbIndex = static_cast<unsigned short>( mb.button );
        m_mouseData.setState( mbIndex, isUp );
    }
    else if( event.type == SDL_MOUSEMOTION )
    {
        m_mouseData.setPos( event.button.x, event.button.y );
    }
    else if( event.type == SDL_MOUSEWHEEL )
    {
        const auto we = event.wheel;
        m_mouseData.setWheel( we.x, we.y, we.direction == SDL_MOUSEWHEEL_NORMAL ? LOGLW::WheelDirection::UP : LOGLW::WheelDirection::DOWN );
    }
}

void SDL2WrapperImpl::handleWindowEvent( const SDL_Event& sdlEvent )
{
    auto eventType = WindowEventType::NONE;

    if( sdlEvent.type == SDL_QUIT )
    {
        eventType = WindowEventType::CLOSE;
    }
    else if( sdlEvent.type == SDL_WINDOWEVENT )
    {
        if( sdlEvent.window.event == SDL_WINDOWEVENT_MOVED )
        {
            eventType = WindowEventType::MOVED;
            m_mainWindow->fetchSreenData();
        }
    }

    notifyWindowEventListeners( eventType );
    notifyWindowEventCallbacks( eventType );
}

void SDL2WrapperImpl::notifyKeyboardCallbacks( const KeyboardState& key )
{
    for( auto callback : m_keyCallbacks )
    {
        callback( key );
    }
}

void SDL2WrapperImpl::registerKeyboardEventCallback( const std::function<void( const KeyboardState& key )>& callback )
{
    m_keyCallbacks.push_back( callback );
}

void SDL2WrapperImpl::registerWindowEventCallback( const WindowCallback& callback )
{
    m_winEventCallbacks.push_back( callback );
}

void SDL2WrapperImpl::stopEventLoop()
{
    eventLoopActive = false;
}

void SDL2WrapperImpl::registerKeyboardEventListener( IKeyboardObserver* observer )
{
    std::lock_guard<std::mutex> lock( m_keyboardObserversMtx );
    m_keyboardObservers.insert( observer );
}

void SDL2WrapperImpl::unregisterKeyboardEventListener( IKeyboardObserver* observer )
{
    std::lock_guard<std::mutex> lock( m_keyboardObserversMtx );
    m_keyboardObservers.erase( observer );
}

void SDL2WrapperImpl::notifyKeyboardListeners( const KeyboardState& key )
{
    std::lock_guard<std::mutex> lock( m_keyboardObserversMtx );
    for( auto listener : m_keyboardObservers )
    {
        listener->onKeyBoardEvent( key );
    }
}

void SDL2WrapperImpl::notifyMouseListerners( const MouseData& md )
{
    std::lock_guard<std::mutex> lock( m_mouseObserversMtx );
    for( const auto& listener : m_mouseObservers )
    {
        listener->onMouseEvent( md );
    }
}

void SDL2WrapperImpl::notifyMouseCallbacks( const MouseData& md )
{
    std::lock_guard<std::mutex> lock( m_mouseObserversMtx );
    for( const auto& listener : m_mouseCallbacks )
    {
        listener( md );
    }
}

void SDL2WrapperImpl::registerMouseEventListener( IMouseObserver* observer )
{
    std::lock_guard<std::mutex> lock( m_mouseObserversMtx );
    m_mouseObservers.insert( observer );
}

void SDL2WrapperImpl::unregisterMouseEventListener( IMouseObserver* observer )
{
    std::lock_guard<std::mutex> lock( m_mouseObserversMtx );
    m_mouseObservers.erase( observer );
}

void SDL2WrapperImpl::registerOnInitCallback( const InitCallback& callback )
{
    m_onInitCallback = callback;
}

void SDL2WrapperImpl::registerWindowEventListener( IWindowEventObserver* observer )
{
    m_windowEventObservers.insert( observer );
}

void SDL2WrapperImpl::unregisterWindowEventListener( IWindowEventObserver* observer )
{
    m_windowEventObservers.erase( observer );
}

void SDL2WrapperImpl::addMouseEventCallback( const MouseCallback& callback )
{
    m_mouseCallbacks.push_back( callback );
}

MouseData& SDL2WrapperImpl::getMouseData()
{
    return m_mouseData;
}

void SDL2WrapperImpl::notifyWindowEventListeners( const WindowEventType e )
{
    for( auto listener : m_windowEventObservers )
    {
        listener->onWindowEvent( e );
    }
}

void SDL2WrapperImpl::notifyWindowEventCallbacks( const WindowEventType e )
{
    for( auto callback : m_winEventCallbacks )
    {
        callback( e );
    }
}

unsigned int SDL2WrapperImpl::getInputLatency() const
{
    return m_eventLatencyUs.getValCopy();
}

void SDL2WrapperImpl::setInputLatency( unsigned latencyInUs )
{
    m_eventLatencyUs = latencyInUs;
}

bool SDL2WrapperImpl::isKeyUp( const String& keyName ) const
{
    return !m_keys.at( keyName );
}

KeyboardState& SDL2WrapperImpl::getKeyStates()
{
    return m_keys;
}

ISprite* SDL2WrapperImpl::createSprite( const CUL::FS::Path& objPath, IWindow* targetWindow ) const
{
    return targetWindow->createSprite( objPath );
}

CUL::Graphics::ITexture* SDL2WrapperImpl::createTexture( const CUL::FS::Path& objPath, IWindow* targetWindow ) const
{
    if( IWindow::Type::SDL_WIN == targetWindow->getType() )
    {
        return targetWindow->createTexture( objPath );
    }
    return nullptr;
}

ISprite* SDL2WrapperImpl::createSprite( CUL::Graphics::ITexture* tex, IWindow* targetWindow ) const
{
    if( IWindow::Type::SDL_WIN == targetWindow->getType() )
    {
        return targetWindow->createSprite( tex );
    }
    return nullptr;
}

IWindow* SDL2WrapperImpl::getMainWindow() const
{
    return m_mainWindow;
}

void SDL2WrapperImpl::registerSDLEventObserver( ISDLEventObserver* eventObserver )
{
    std::lock_guard<std::mutex> lock( m_sdlEventObserversMtx );
    m_sdlEventObservers.insert( eventObserver );
}

void SDL2WrapperImpl::unRegisterSDLEventObserver( ISDLEventObserver* eventObserver )
{
    std::lock_guard<std::mutex> lock( m_sdlEventObserversMtx );
    m_sdlEventObservers.erase( eventObserver );
}

SDL2WrapperImpl::~SDL2WrapperImpl()
{
    m_keys.clear();
    SDL_Quit();
}

#ifdef _MSC_VER
    #pragma warning( pop )
#endif