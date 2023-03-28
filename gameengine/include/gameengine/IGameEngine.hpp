#pragma once

#include "gameengine/IObjectFactory.hpp"

#include "SDL2Wrapper/Input/IKeyboardObservable.hpp"
#include "SDL2Wrapper/IWindowEventObservable.hpp"
#include "SDL2Wrapper/Input/IMouseObservable.hpp"

#include "CUL/Graphics/Color.hpp"

#include "CUL/GenericUtils/ITask.hpp"
#include "CUL/Graphics/IImageLoader.hpp"
#include "CUL/Log/ILogContainer.hpp"
#include "CUL/Math/Vector3D.hpp"
#include "CUL/String.hpp"
#include "CUL/GenericUtils/SimpleDelegate.hpp"

#include "CUL/STL_IMPORTS/STD_thread.hpp"
#include "CUL/STL_IMPORTS/STD_queue.hpp"
#include "CUL/STL_IMPORTS/STD_atomic.hpp"
#include "CUL/STL_IMPORTS/STD_stack.hpp"

NAMESPACE_BEGIN( CUL )
NAMESPACE_BEGIN( GUTILS )
class IConfigFile;
#ifdef _MSC_VER
class GAME_ENGINE_API CUL::GUTILS::SimpleDelegate;
#endif
NAMESPACE_END( GUTILS )
NAMESPACE_END( CUL )

NAMESPACE_BEGIN( SDL2W )
class IWindow;
class ISDL2Wrapper;
struct WinSize;
NAMESPACE_END( SDL2W )

struct ImGuiContext;

NAMESPACE_BEGIN( LOGLW )

class IDebugOverlay;
class ProjectionData;
class ITextureFactory;
class Camera;
class Cube;
class Sprite;
class Quad;
class Triangle;
class Viewport;
class IRenderDevice;
class Shader;
struct EngineParams;

using String = CUL::String;

using ColorS = CUL::Graphics::ColorS;
using ColorE = CUL::Graphics::ColorE;

using Vector3Di = CUL::MATH::Vector3Di;
using CMString = const String;
using IImageLoader = CUL::Graphics::IImageLoader;
using EmptyFunctionCallback = std::function<void()>;
using IPreRenderTask = CUL::GUTILS::ITask;
using Pos3Df = CUL::Graphics::Pos3Dd;

class IGameEngine: public SDL2W::IMouseObservable,
                   public SDL2W::IKeyboardObservable,
                   public SDL2W::IWindowEventObservable,
                   private IObjectFactory
{
public:
    IGameEngine();

    GAME_ENGINE_API virtual void initialize();
    GAME_ENGINE_API virtual void renderFrame() = 0;
    GAME_ENGINE_API virtual void runEventLoop() = 0;
    GAME_ENGINE_API virtual void stopEventLoop() = 0;

    GAME_ENGINE_API virtual SDL2W::IWindow* getMainWindow() = 0;

    GAME_ENGINE_API virtual void setBackgroundColor( const ColorS& color ) = 0;
    GAME_ENGINE_API virtual void startRenderingLoop() = 0;
    GAME_ENGINE_API virtual void stopRenderingLoop() = 0;
    GAME_ENGINE_API virtual void onInitialize( const EmptyFunctionCallback& callback ) = 0;

    GAME_ENGINE_API IObjectFactory* getObjectFactory();
    GAME_ENGINE_API virtual IImageLoader* getImageLoader() = 0;
    GAME_ENGINE_API virtual IRenderDevice* getDevice() = 0;
    GAME_ENGINE_API virtual const Viewport& getViewport() const = 0;

    GAME_ENGINE_API virtual CUL::CULInterface* getCul() = 0;
    GAME_ENGINE_API virtual CUL::LOG::ILogger* getLoger() = 0;

    GAME_ENGINE_API Camera& getCamera();

    GAME_ENGINE_API virtual void beforeFrame( const EmptyFunctionCallback& callback ) = 0;
    GAME_ENGINE_API virtual void setProjection( const Camera& rect ) = 0;
    GAME_ENGINE_API virtual void setEyePos( const glm::vec3& pos ) = 0;
    GAME_ENGINE_API virtual void setViewport( const Viewport& viewport, const bool instant = false ) = 0;

    GAME_ENGINE_API virtual void drawQuad( const bool draw = true ) = 0;

    GAME_ENGINE_API virtual void clearModelViewEveryFrame( const bool enable ) = 0;

    GAME_ENGINE_API virtual const ContextInfo& getContext() const = 0;

    GAME_ENGINE_API virtual void drawDebugInfo( const bool enable ) = 0;
    GAME_ENGINE_API void drawOrigin( bool enable );

    GAME_ENGINE_API virtual IDebugOverlay* getDebugOverlay() = 0;

    GAME_ENGINE_API virtual CUL::GUTILS::IConfigFile* getConfig() = 0;

    GAME_ENGINE_API virtual ITextureFactory* getTextureFactory() = 0;

    // VBO HANDLE:
    GAME_ENGINE_API virtual class VertexBuffer* createVBO( std::vector<float>& data ) = 0;

    GAME_ENGINE_API static IGameEngine* createGameEngine( SDL2W::ISDL2Wrapper* sdl2w, bool legacy = false );
    GAME_ENGINE_API static IGameEngine* createGameEngine( const EngineParams& engineParam );

    GAME_ENGINE_API static IGameEngine* getInstance();

    GAME_ENGINE_API virtual void setFpsLimit( float maxFps ) = 0;
    GAME_ENGINE_API virtual float getFpsLimit() const = 0;

    // Object Factory
    GAME_ENGINE_API Sprite* createSprite();
    GAME_ENGINE_API Line* createLine( IObject* parent, bool forceLegacy );
    GAME_ENGINE_API Triangle* createTriangle( IObject* parent, bool forceLegacy = false );
    GAME_ENGINE_API Quad* createQuad( IObject* parent, bool forceLegacy = false );
    GAME_ENGINE_API class VertexArray* createVAO();
    GAME_ENGINE_API Cube* createCube( bool forceLegacy = false );

    GAME_ENGINE_API void pushPreRenderTask( IPreRenderTask* preRenderTask );
    GAME_ENGINE_API void pushPreRenderTask( std::function<void( void )> task );

    GAME_ENGINE_API void addObjectToRender( IRenderable* renderable );
    GAME_ENGINE_API void removeObjectToRender( IRenderable* renderable );

    GAME_ENGINE_API void toggleGrid(bool enableGrid);

    GAME_ENGINE_API unsigned getGPUTotalAvailableMemoryKb();
    GAME_ENGINE_API unsigned getGPUCurrentAvailableMemoryKb();

    GAME_ENGINE_API virtual void addRenderThreadTask( const std::function<void( void )>& task ) = 0;

    GAME_ENGINE_API ImGuiContext* getGuiContext() const;
    GAME_ENGINE_API void setGuiContext( ImGuiContext* const inContext );

    CUL::GUTILS::SimpleDelegate guiFrameDelegate;


    // Shaders
    GAME_ENGINE_API class Program* createProgram();
    GAME_ENGINE_API void removeProgram(Program* program);

    GAME_ENGINE_API Shader* createShader( const String& path, const String& source = "" );
    GAME_ENGINE_API void removeShader( Shader* shader );
    GAME_ENGINE_API void removeShader( const String& path );

    GAME_ENGINE_API void releaseResources();

    GAME_ENGINE_API void addGuiTask( std::function<void(void)> task );

    GAME_ENGINE_API virtual ~IGameEngine();

protected:
    std::atomic_bool m_drawOrigin = false;
    std::array<IObject*, 3> m_axis = { nullptr, nullptr, nullptr };
    struct GUIParams
    {
        float z = 0.0f;
    };

    GUIParams m_guiParams;

    IRenderDevice* m_renderDevice = nullptr;
    std::mutex m_preRenderTasksMtx;
    std::queue<IPreRenderTask*> m_preRenderTasks;
    std::queue<std::function<void( void )>> m_preRenderTasksFunction;

    std::mutex m_objectsToRenderMtx;
    std::set<IRenderable*> m_objectsToRender;
    bool m_gridEnabled = false;

    std::mutex m_guiTasksMtx;
    std::queue<std::function<void( void )>> m_guiTasks;


    std::mutex m_initTasksMtx;
    std::stack< std::function<void( void )>> m_initTasks;

private:
    Shader* findShader( const String& path ) const;

    std::unique_ptr<Camera> m_camera;
    bool m_forceLegacy = false;

    IGameEngine( const IGameEngine& val ) = delete;
    IGameEngine( IGameEngine&& val ) = delete;
    IGameEngine& operator=( const IGameEngine& rhv ) = delete;
    IGameEngine& operator=( IGameEngine&& rhv ) = delete;

    static IGameEngine* s_instance;

    std::map<Program*, std::unique_ptr<Program>> m_shadersPrograms;
    std::map<String, Shader*> m_shaders;

    ImGuiContext* m_ImGuiContext = nullptr;
};

NAMESPACE_END( LOGLW )