#pragma once
#include "SDL2Wrapper/ISDL2Wrapper.hpp"
#include "SDL2Wrapper/IWindow.hpp"
#include "SDL2Wrapper/ISDLEventObserver.hpp"
#include "SDL2Wrapper/Input/MouseData.hpp"

#include "WindowFactoryConcrete.hpp"
#include "RegularSDL2Window.hpp"

#include "CUL/CULInterface.hpp"
#include "CUL/GenericUtils/LckPrim.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"
#include "CUL/STL_IMPORTS/STD_set.hpp"
#include "CUL/GenericUtils/DumbPtr.hpp"

struct SDL_Surface;
struct SDL_Renderer;
union SDL_Event;

NAMESPACE_BEGIN( SDL2W )

using WindowCollection = std::map<unsigned int, std::unique_ptr<IWindow>>;
template <typename TYPE> using LckPrim = CUL::GUTILS::LckPrim<TYPE>;
template <typename TYPE> using DumbPtr = CUL::GUTILS::DumbPtr<TYPE>;

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4820 )
#endif

class SDL2WrapperImpl final:
    public ISDL2Wrapper
{
public:
    SDL2WrapperImpl();

    ~SDL2WrapperImpl();
protected:
private:
    void init( const WindowData& wd, const CUL::FS::Path& configPath = "" ) override;
    size_t fetchRenderTypes();

    void registerSDLEventObserver( ISDLEventObserver* eventObserver ) override;
    void unRegisterSDLEventObserver( ISDLEventObserver* eventObserver ) override;

    CUL::CULInterface* getCul() override;

    IConfigFile* getConfig() const override;
    void refreshScreen() override;
    void renderFrame( bool clearContext = true, bool refreshWindow = true ) override;
    void clearWindows() override;

    void printAvailableRenderers() const override;

    void runEventLoop() override;
    void stopEventLoop() override;
    void pollEvents() override;

    void notifySDLEventObservers( SDL_Event& event );

    void registerKeyboardEventCallback( const std::function<void( const KeyboardState& key )>& callback ) override;
    void registerWindowEventCallback( const WindowCallback& callback ) override;

    void registerKeyboardEventListener( IKeyboardObserver* observer ) override;
    void unregisterKeyboardEventListener( IKeyboardObserver* observer ) override;

    void registerWindowEventListener( IWindowEventObserver* observer ) override;
    void unregisterWindowEventListener( IWindowEventObserver* observer ) override;

    void addMouseEventCallback( const MouseCallback& callback ) override;

    void registerMouseEventListener( IMouseObserver* observer ) override;
    void unregisterMouseEventListener( IMouseObserver* observer ) override;

    void registerOnInitCallback( const InitCallback& callback ) override;

    MouseData& getMouseData() override;

    unsigned int getInputLatency() const override;
    void setInputLatency( unsigned latencyInUs ) override;
    bool isKeyUp( const String& keyName ) const override;
    KeyboardState& getKeyStates() override;

    IWindow* getMainWindow() const override;

    void createKeys();

    CUL::GUTILS::DumbPtr<CUL::CULInterface> m_culInterface;
    CUL::LOG::ILogger* getLogger() const override;

    void handleEvent( const SDL_Event& event );

    static bool isMouseEvent( const SDL_Event& event );
    static bool isWindowEvent( const SDL_Event& event );

    void handleKeyboardEvent( const SDL_Event& sdlEvent );
    void handleMouseEvent( const SDL_Event& sdlEvent );
    void handleWindowEvent( const SDL_Event& sdlEvent );

    void notifyKeyboardCallbacks( const KeyboardState& key );
    void notifyKeyboardListeners( const KeyboardState& key );
    void notifyMouseListerners( const MouseData& md );
    void notifyMouseCallbacks( const MouseData& md );
    void notifyWindowEventListeners( const WindowEventType e );
    void notifyWindowEventCallbacks( const WindowEventType e );

    CUL::Graphics::ITexture* createTexture( const CUL::FS::Path& path, IWindow* targetWindow ) const override;
    ISprite* createSprite( const CUL::FS::Path& path, IWindow* targetWindow ) const override;
    ISprite* createSprite( CUL::Graphics::ITexture* tex, IWindow* targetWindow ) const override;

    DumbPtr<WindowCreatorConcrete> m_windowFactory;
    WindowData m_windowData;
    SDL_Renderer* m_renderer = nullptr;

    LckPrim<bool> eventLoopActive = true;
    LckPrim<unsigned int> m_eventLatencyUs = 256;

    std::set<IWindowEventObserver*> m_windowEventObservers;
    RegularSDL2Window* m_mainWindow = nullptr;
    WindowCollection m_windows;

    KeyboardState m_keys;

    CUL::LOG::ILogger* m_logger = nullptr;

    MouseData m_mouseData;

    InitCallback m_onInitCallback;

    std::set<IKeyboardObserver*> m_keyboardObservers;
    std::vector<std::function<void( const KeyboardState& key )>> m_keyCallbacks;
    std::mutex m_keyboardObserversMtx;

    std::set<IMouseObserver*> m_mouseObservers;
    std::vector<std::function<void( const MouseData& md )>> m_mouseCallbacks;
    std::mutex m_mouseObserversMtx;

    std::vector<std::function<void( const WindowEventType wEt )>> m_winEventCallbacks;

    std::set<ISDLEventObserver*> m_sdlEventObservers;
    std::mutex m_sdlEventObserversMtx;

    IConfigFile* m_configFile = nullptr;

private: // Deleted methods.
    SDL2WrapperImpl( const SDL2WrapperImpl& rhv ) = delete;
    SDL2WrapperImpl( SDL2WrapperImpl&& rhv ) = delete;
    SDL2WrapperImpl& operator=( const SDL2WrapperImpl& rhv ) = delete;
    SDL2WrapperImpl& operator=( SDL2WrapperImpl&& rhv ) = delete;
};

#ifdef _MSC_VER
#pragma warning( pop )
#endif

NAMESPACE_END( SDL2W )
