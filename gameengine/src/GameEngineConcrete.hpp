#pragma once

#include "gameengine/IGameEngine.hpp"
#include "gameengine/IDebugOverlay.hpp"
#include "gameengine/ITextureFactory.hpp"
#include "gameengine/IObjectFactory.hpp"
#include "gameengine/Sprite.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Viewport.hpp"

#include "SDL2Wrapper/IMPORT_SDL_video.hpp"
#include "SDL2Wrapper/ISDLEventObserver.hpp"

#include "CUL/GenericUtils/LckPrim.hpp"
#include "CUL/Threading/ThreadUtils.hpp"
#include "CUL/GenericUtils/DumbPtr.hpp"
#include "CUL/JSON/IJSONFile.hpp"
#include "CUL/ITimer.hpp"
#include "CUL/GenericUtils/Version.hpp"

#include "CUL/STL_IMPORTS/STD_set.hpp"
#include "CUL/STL_IMPORTS/STD_array.hpp"
#include "CUL/STL_IMPORTS/STD_thread.hpp"
#include "CUL/STL_IMPORTS/STD_variant.hpp"
#include "CUL/STL_IMPORTS/STD_atomic.hpp"
#include "CUL/STL_IMPORTS/STD_stack.hpp"

union SDL_Event;

NAMESPACE_BEGIN( SDL2W )
class IKey;
class IKeyboardObserver;
NAMESPACE_END( SDL2W )

NAMESPACE_BEGIN( LOGLW )

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

class GameEngineConcrete final: public IGameEngine,
                                private IObjectFactory,
                                private IDebugOverlay,
                                private SDL2W::ISDLEventObserver,
                                private ITextureFactory
{
public:
    GameEngineConcrete( SDL2W::ISDL2Wrapper* sdl2w, bool legacy );
    void registerObjectForUtility();
    ~GameEngineConcrete();

protected:
private:
    void initialize() override;
    void loadFromConfig();
    void showExtensions();
    void setupProjectionData( const SDL2W::WinSize& winSize );
    CUL::CULInterface* getCul() override;

    void refreshBuffers();
    void setProjection( const Camera& rect ) override;
    void setViewport( const Viewport& viewport, const bool instant = false ) override;

    IObject* createFromFile( const String& path ) override;
    IObject* createFromFile( CUL::JSON::IJSONFile* file );
    IObject* createFromFile( IFile* file ) override;
    IObject* createTriangle( CUL::JSON::INode* jNode );
    ITriangle* createTriangle( const TriangleData& data, const ColorS& color = ColorE::WHITE ) override;
    IQuad* createQuad( const QuadData& data, bool legacy = false, const ColorS& color = ColorE::WHITE ) override;
    ILine* createLine( const LineData& data, const ColorS& color = ColorE::WHITE ) override;

    IPoint* createPoint( const Point& position, const ColorS& color = ColorE::WHITE ) override;

    Sprite* createSprite( const String& path, bool withVBO = false ) override;
    Sprite* createSprite( unsigned* data, unsigned width, unsigned height, bool withVBO = false ) override;

    void removeObject( IObject* object ) override;

    void setBackgroundColor( const ColorS& color ) override;
    void startRenderingLoop() override;
    void stopRenderingLoop() override;
    void onInitialize( const EmptyFunctionCallback& callback ) override;
    void beforeFrame( const EmptyFunctionCallback& callback ) override;

    IObjectFactory* getObjectFactory() override;
    IImageLoader* getImageLoader() override;
    CUL::LOG::ILogger* getLoger() override;
    IUtility* getUtility() override;
    const Viewport& getViewport() const override;
    // ProjectionData& getProjectionData() override;

    // VBO HANDLE:
    VertexBuffer* createVBO( std::vector<float>& data ) override;

    const ContextInfo& getContext() const override;

    void mainThread();
    void renderLoop();
    void taskThread();
    void calculateNextFrameLengths();
    void initDebugInfo();
    void renderFrame() override;
    void renderInfo();
    void changeProjectionType();
    void prepareProjection();
    void setEyePos( const glm::vec3& pos ) override;
    void renderObjects();

    void executeTasks();

    void release();

    void drawQuad( const bool draw = true ) override;

    void clearModelViewEveryFrame( const bool enable ) override;
    void calculateFrameWait();

    CUL::GUTILS::IConfigFile* getConfig() override;

    void drawDebugInfo( const bool enable ) override;
    void drawOrigin( bool enable ) override;

    IDebugOverlay* getDebugOverlay() override;
    void handleEvent( const SDL_Event& event ) override;

    unsigned addSliderValue( const CUL::String& valName, float* value, float min, float max,
                             const std::function<void( void )>& onUpdate = nullptr ) override;
    unsigned addText( const CUL::String& text, float* value ) override;

    void runEventLoop() override;
    void stopEventLoop() override;
    SDL2W::IWindow* getMainWindow() override;

    // ITextureFactory
    ITextureFactory* getTextureFactory() override;
    ITexture* createTexture( const CUL::FS::Path& path, const bool rgba = false ) override;

    // SDL2W::IMouseObservable
    void addMouseEventCallback( const SDL2W::IMouseObservable::MouseCallback& callback ) override;
    void registerMouseEventListener( SDL2W::IMouseObserver* observer ) override;
    void unregisterMouseEventListener( SDL2W::IMouseObserver* observer ) override;
    SDL2W::MouseData& getMouseData() override;

    // SDL2W::IKeyboardObservable
    void registerKeyboardEventCallback( const std::function<void( const SDL2W::IKey& key )>& callback ) override;

    void registerKeyboardEventListener( SDL2W::IKeyboardObserver* observer ) override;
    void unregisterKeyboardEventListener( SDL2W::IKeyboardObserver* observer ) override;

    bool isKeyUp( const String& keyName ) const override;

    void registerWindowEventCallback( const SDL2W::WindowCallback& callback ) override;

    void addRenderThreadTask( const std::function<void( void )>& task ) override;

    std::map<unsigned, DebugValueRow> m_debugValues;

    std::atomic<bool> m_enableDebugDraw = false;
    std::atomic<bool> m_debugDrawInitialized = false;

    ContextInfo m_glContext;

    SDL2W::ISDL2Wrapper* m_sdlW = nullptr;
    SDL2W::IWindow* m_activeWindow = nullptr;
    CUL::CULInterface* m_cul = nullptr;
    CUL::LOG::ILogger* m_logger = nullptr;
    IUtility* m_oglUtility = nullptr;

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

    SafeBool m_projectionChanged = false;

    ColorS m_backgroundColor = ColorS( ColorE::BLACK );

    std::mutex m_taskMutex;
    std::stack<std::function<void( void )>> m_tasks;

    EmptyFunctionCallback m_onInitializeCallback;
    EmptyFunctionCallback m_onBeforeFrame;

    void setFpsLimit( float maxFps ) override;
    float getFpsLimit() const override;

    DumbPtr<CUL::ITimer> m_frameTimer;
    Safe<float> m_fpsLimit = 60.f;
    Safe<int> m_targetFrameLengthUs = 0u;
    Safe<int> m_currentFrameLengthUs = 0u;
    Safe<int> m_frameSleepUs = 64;
    int m_usRes = 0;
    int m_usDelta = 8;

    Safe<int> m_waitTimeUs = 0.0f;

    bool m_hasBeenInitialized = false;
    bool m_userInitialized = false;
    SafeBool m_drawQuad = false;
    SafeBool m_drawOrigin = false;

    std::array<IObject*, 3> m_axis = { nullptr, nullptr, nullptr };

    std::map<String, CUL::GUTILS::Version> m_renderersVersions;

private:  // Deleted
    GameEngineConcrete() = delete;
    GameEngineConcrete( const GameEngineConcrete& val ) = delete;
    GameEngineConcrete( GameEngineConcrete&& val ) = delete;
    GameEngineConcrete& operator=( const GameEngineConcrete& rhv ) = delete;
    GameEngineConcrete& operator=( GameEngineConcrete&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )