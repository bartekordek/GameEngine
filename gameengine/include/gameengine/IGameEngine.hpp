#pragma once

#include "gameengine/IObjectFactory.hpp"
#include "gameengine/IProgramFactory.hpp"
#include "gameengine/IShaderFactory.hpp"
#include "gameengine/IUtility.hpp"
#include "gameengine/Viewport.hpp"
#include "gameengine/VertexArray.hpp"

#include "SDL2Wrapper/ISDL2Wrapper.hpp"
#include "SDL2Wrapper/IWindow.hpp"
#include "SDL2Wrapper/IWindowEventObservable.hpp"
#include "SDL2Wrapper/Input/IMouseObservable.hpp"

#include "CUL/Graphics/Color.hpp"
#include "CUL/GenericUtils/ITask.hpp"
#include "CUL/STL_IMPORTS/STD_thread.hpp"
#include "CUL/STL_IMPORTS/STD_queue.hpp"
#include "CUL/Graphics/IImageLoader.hpp"
#include "CUL/Log/ILogContainer.hpp"
#include "CUL/Math/Vector3D.hpp"
#include "CUL/String.hpp"

NAMESPACE_BEGIN( CUL )
NAMESPACE_BEGIN( GUTILS )
class IConfigFile;
NAMESPACE_END( GUTILS )
NAMESPACE_END( CUL )

NAMESPACE_BEGIN( LOGLW )

class IDebugOverlay;
class ProjectionData;
class ITextureFactory;
class Camera;
class Cube;
class Sprite;
class Quad;

using String = CUL::String;

using ColorS = CUL::Graphics::ColorS;
using ColorE = CUL::Graphics::ColorE;

using Vector3Di = CUL::MATH::Vector3Di;
using CMString = const String;
using IImageLoader = CUL::Graphics::IImageLoader;
using EmptyFunctionCallback = std::function<void()>;
using IPreRenderTask = CUL::GUTILS::ITask;
using Pos3Df = CUL::Graphics::Pos3Dd;

class GAME_ENGINE_API IGameEngine: public SDL2W::IMouseObservable, public SDL2W::IKeyboardObservable, public SDL2W::IWindowEventObservable
{
public:
    IGameEngine();

    virtual void initialize();
    virtual void renderFrame() = 0;
    virtual void runEventLoop() = 0;
    virtual void stopEventLoop() = 0;

    virtual SDL2W::IWindow* getMainWindow() = 0;

    virtual void setBackgroundColor( const ColorS& color ) = 0;
    virtual void startRenderingLoop() = 0;
    virtual void stopRenderingLoop() = 0;
    virtual void onInitialize( const EmptyFunctionCallback& callback ) = 0;

    virtual IShaderFactory* getShaderFactory() = 0;
    virtual IObjectFactory* getObjectFactory() = 0;
    virtual IProgramFactory* getProgramFactory() = 0;
    virtual IImageLoader* getImageLoader() = 0;
    virtual IUtility* getUtility() = 0;
    virtual const Viewport& getViewport() const = 0;
    // virtual ProjectionData& getProjectionData() = 0;

    virtual CUL::CULInterface* getCul() = 0;
    virtual CUL::LOG::ILogger* getLoger() = 0;

    Camera* getCamera();

    virtual void beforeFrame( const EmptyFunctionCallback& callback ) = 0;

    virtual void setProjection( const Camera& rect ) = 0;
    virtual void setEyePos( const glm::vec3& pos ) = 0;
    virtual void setViewport( const Viewport& viewport, const bool instant = false ) = 0;

    virtual void drawQuad( const bool draw = true ) = 0;

    virtual void clearModelViewEveryFrame( const bool enable ) = 0;

    virtual const ContextInfo& getContext() const = 0;

    virtual void drawDebugInfo( const bool enable ) = 0;
    virtual void drawOrigin( bool enable ) = 0;

    virtual IDebugOverlay* getDebugOverlay() = 0;

    virtual CUL::GUTILS::IConfigFile* getConfig() = 0;

    virtual ITextureFactory* getTextureFactory() = 0;

    // VBO HANDLE:
    virtual VertexBuffer* createVBO( std::vector<float>& data ) = 0;

    static IGameEngine* createGameEngine( SDL2W::ISDL2Wrapper* sdl2w, bool legacy = false );
    static IGameEngine* createGameEngine( bool legacy, const CUL::Graphics::Pos2Di& pos, const SDL2W::WindowSize& winSize,
                                          const String& configPath, const String& winName = "", const String& renderername = "opengl" );

    static IGameEngine* getInstance();

    virtual void setFpsLimit( float maxFps ) = 0;
    virtual float getFpsLimit() const = 0;

    // Object Factory
    Sprite* createSprite();
    Quad* createQuad( IObject* parent );
    VertexArray* createVAO();
    Cube* createCube();

    void pushPreRenderTask( IPreRenderTask* preRenderTask );
    void pushPreRenderTask( std::function<void( void )> task );

    void addObjectToRender( IRenderable* renderable );
    void removeObjectToRender( IRenderable* renderable );

    virtual ~IGameEngine();

protected:
    std::mutex m_preRenderTasksMtx;
    std::queue<IPreRenderTask*> m_preRenderTasks;
    std::queue<std::function<void( void )>> m_preRenderTasksFunction;

    std::mutex m_objectsToRenderMtx;
    std::set<IRenderable*> m_objectsToRender;

private:
    std::unique_ptr<Camera> m_camera;
    bool m_forceLegacy = false;

    IGameEngine( const IGameEngine& val ) = delete;
    IGameEngine( IGameEngine&& val ) = delete;
    IGameEngine& operator=( const IGameEngine& rhv ) = delete;
    IGameEngine& operator=( IGameEngine&& rhv ) = delete;

    static IGameEngine* s_instance;
};

NAMESPACE_END( LOGLW )