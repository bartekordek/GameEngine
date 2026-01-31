#pragma once

#include "gameengine/IGameEngine.hpp"
#include "gameengine/IDebugOverlay.hpp"
#include "gameengine/ITextureFactory.hpp"
#include "gameengine/Sprite.hpp"
#include "gameengine/String.hpp"
#include "gameengine/Viewport.hpp"

#include "gameengine/Events/ISDLEventObserver.hpp"
#include "gameengine/IRenderDevice.hpp"

#include "CUL/GenericUtils/LckPrim.hpp"
#include "CUL/Threading/ThreadUtil.hpp"
#include "CUL/GenericUtils/DumbPtr.hpp"
#include "CUL/JSON/IJSONFile.hpp"
#include "CUL/ITimer.hpp"
#include "CUL/GenericUtils/Version.hpp"
#include "CUL/Math/VariableWithAverageValue.hpp"

#include "CUL/STL_IMPORTS/STD_set.hpp"
#include "CUL/STL_IMPORTS/STD_array.hpp"
#include "CUL/STL_IMPORTS/STD_thread.hpp"
#include "CUL/STL_IMPORTS/STD_variant.hpp"
#include "CUL/STL_IMPORTS/STD_atomic.hpp"

union SDL_Event;

NAMESPACE_BEGIN( LOGLW )
class Key;
class IKeyboardObserver;
NAMESPACE_END( LOGLW )

NAMESPACE_BEGIN( LOGLW )

class Camera;
class DebugSystemBase;

using SafeBool = CUL::GUTILS::LckPrim<bool>;
template <typename Type>
using DumbPtr = CUL::GUTILS::DumbPtr<Type>;
template <typename Type>
using Safe = CUL::GUTILS::LckPrim<Type>;

enum class DebugType
{
    TEXT = 0,
    SLIDER
};

enum class DataTypes
{
    NONE = -1,
    INT,
    UNSIGNED_INT,
    STRING,
    FLOAT,
    DOUBLE
};

struct DebugValueRow
{
    unsigned id = 0;
    String text;
    std::variant<String*, float*, int*> value;
    DebugType type = DebugType::TEXT;
    float min = 0.0f;
    float max = 0.0f;
    std::function<void( void )> onChange;
};

class GameEngineConcrete final: public IGameEngine, private IDebugOverlay, private LOGLW::ISDLEventObserver, private ITextureFactory
{
public:
    GameEngineConcrete( LOGLW::ISDL2Wrapper* sdl2w, bool legacy );
    void registerObjectForUtility();
    ~GameEngineConcrete();

protected:
private:
    void initialize() override;
    void loadDebugDraw();
    void showExtensions();
    void setupProjectionData( uint16_t width, uint16_t height );
    CUL::CULInterface* getCul() override;
    bool drawObjectsInfo( float& width, float& high ) override;

    void finishFrame();
    void setViewport( const Viewport& viewport, const bool instant = false ) override;

    Sprite* createSprite( const String& path, bool withVBO = false ) override;
    Sprite* createSprite( unsigned* data, unsigned width, unsigned height, bool withVBO = false ) override;

    void removeObject( IObject* object ) override;

    void setBackgroundColor( const ColorS& color ) override;
    void startRenderingLoop() override;
    void stopRenderingLoop() override;
    void onInitialize( const EmptyFunctionCallback& callback ) override;
    void beforeFrame( const EmptyFunctionCallback& callback ) override;

    IImageLoader* getImageLoader() override;
    CUL::LOG::ILogger* getLoger() override;
    IRenderDevice* getDevice() override;
    const Viewport& getViewport() const override;
    // ProjectionData& getProjectionData() override;

    const ContextInfo& getContext() const override;

    void mainThread();
    void renderLoop();

    void runPreRenderTasks();
    void runPostRenderTasks();

    void taskThread();
    void renderFrame() override;
    void renderInfo();
    void changeProjectionType();
    void prepareProjection();
    void setEyePos( const glm::vec3& pos ) override;
    void renderObjects();
    void drawObjects( std::set<IObject*>& shownList, IObject* currentObject, const String& name );
    bool drawValues( glm::vec3& inOutValue, const String& inName );
    void drawSpriteData( Sprite* inSprite );

    void release();

    void drawQuad( const bool draw = true ) override;

    void clearModelViewEveryFrame( const bool enable ) override;

    CUL::GUTILS::IConfigFile* getConfig() override;

    void drawDebugInfo( const bool enable ) override;

    IDebugOverlay* getDebugOverlay() override;
    void handleEvent( const SDL_Event& event ) override;

    void runEventLoop() override;
    void stopEventLoop() override;
    LOGLW::IWindow* getMainWindow() override;

    // ITextureFactory
    ITextureFactory* getTextureFactory() override;
    ITexture* createTexture( const CUL::FS::Path& path, const bool rgba = false ) override;

    // LOGLW::IMouseObservable
    void addMouseEventCallback( const LOGLW::IMouseObservable::MouseCallback& callback ) override;
    void registerMouseEventListener( LOGLW::IMouseObserver* observer ) override;
    void unregisterMouseEventListener( LOGLW::IMouseObserver* observer ) override;
    LOGLW::MouseData& getMouseData() override;

    void registerKeyboardEventCallback( const std::function<void( const LOGLW::KeyboardState& key )>& callback ) override;

    void registerKeyboardEventListener( LOGLW::IKeyboardObserver* observer ) override;
    void unregisterKeyboardEventListener( LOGLW::IKeyboardObserver* observer ) override;

    bool isKeyUp( const String& keyName ) const override;

    void registerWindowEventCallback( const LOGLW::WindowCallback& callback ) override;

    std::unique_ptr<DebugSystemBase> m_debugSystem;

    std::atomic<bool> m_enableDebugDraw = false;

    ContextInfo m_glContext;

    LOGLW::ISDL2Wrapper* m_sdlW = nullptr;
    LOGLW::IWindow* m_activeWindow = nullptr;
    CUL::CULInterface* m_cul = nullptr;
    CUL::LOG::ILogger* m_logger = nullptr;

    IImageLoader* m_imageLoader = nullptr;

    std::thread m_renderingLoopThread;
    std::thread m_taskLoopThread;

    // CUL::GUTILS::ValueChangeHook<bool> m_isPerspective = true;
    SafeBool m_runRenderLoop = true;
    SafeBool m_clearEveryFrame = true;
    SafeBool m_clearModelView = true;
    SafeBool m_updateBuffers = true;

    Viewport m_viewport;
    SafeBool m_viewportChanged = false;

    ColorS m_backgroundColor = ColorS( ColorE::BLACK );

    EmptyFunctionCallback m_onInitializeCallback;
    EmptyFunctionCallback m_onBeforeFrame;

    void setFpsLimit( float maxFps ) override;
    float getFpsLimit() const override;

    DumbPtr<CUL::ITimer> m_customTimer;

    Safe<int> m_waitTimeUs = 0.0f;

    bool m_hasBeenInitialized = false;
    bool m_userInitialized = false;
    SafeBool m_drawQuad = false;

    std::map<String, CUL::GUTILS::Version> m_renderersVersions;

    int m_everyX = 0;
    int m_everyXMax = 4;

private:  // Deleted
    GameEngineConcrete() = delete;
    GameEngineConcrete( const GameEngineConcrete& val ) = delete;
    GameEngineConcrete( GameEngineConcrete&& val ) = delete;
    GameEngineConcrete& operator=( const GameEngineConcrete& rhv ) = delete;
    GameEngineConcrete& operator=( GameEngineConcrete&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )