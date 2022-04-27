#include "gameengine/IGameEngine.hpp"
#include "GameEngineConcrete.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Cube.hpp"

#include "SDL2Wrapper/WindowData.hpp"

using namespace LOGLW;

IGameEngine* IGameEngine::s_instance = nullptr;

IGameEngine::IGameEngine()
{
}

IGameEngine* IGameEngine::createGameEngine( SDL2W::ISDL2Wrapper* sdl2w, bool legacy )
{
    s_instance = new GameEngineConcrete( sdl2w, legacy );
    return s_instance;
}

IGameEngine* IGameEngine::createGameEngine( bool legacy, const CUL::Graphics::Pos2Di& pos, const SDL2W::WindowSize& winSize,
                                            const String& configPath, const String& winName, const String& renderername )
{
    SDL2W::WindowData windowData;
    windowData.name = winName;
    windowData.pos = pos;
    windowData.currentRes = winSize;
    windowData.rendererName = renderername;

    auto sdlWrap = SDL2W::ISDL2Wrapper::createSDL2Wrapper();
    sdlWrap->init( windowData, configPath );

    s_instance = new GameEngineConcrete( sdlWrap, legacy );
    return s_instance;
}

void IGameEngine::initialize()
{
    m_camera = std::make_unique<Camera>( &getProjectionData() );
}

Camera* IGameEngine::getCamera()
{
    return m_camera.get();
}

IGameEngine* IGameEngine::getInstance()
{
    return s_instance;
}

Sprite* IGameEngine::createSprite()
{
    auto sprite = new Sprite( getCamera(), getCul() );

    // sprite->LoadImage( path, m_imageLoader );

    addObjectToRender( sprite );

    return sprite;
}

Quad* IGameEngine::createQuad( IObject* parent )
{
    Quad* result = new Quad( *getCamera(), *this, parent );

    return result;
}

VertexArray* IGameEngine::createVAO()
{
    auto result = new VertexArray( *this );
    return result;
}

Cube* IGameEngine::createCube()
{
    return new Cube(getCamera(), this);
}

void IGameEngine::pushPreRenderTask( IPreRenderTask* preRenderTask )
{
    std::lock_guard<std::mutex> locker( m_preRenderTasksMtx );
    m_preRenderTasks.push(preRenderTask);
}

void IGameEngine::pushPreRenderTask( std::function<void( void )> task )
{
    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        m_preRenderTasksFunction.push( task );
    }
    else
    {
        std::lock_guard<std::mutex> locker( m_preRenderTasksMtx );
        m_preRenderTasksFunction.push( task );
    }
}


void IGameEngine::addObjectToRender( IRenderable* renderable )
{
    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        auto it = m_objectsToRender.find( renderable );
        CUL::Assert::simple( it == m_objectsToRender.end(), "Trying to add already added object." );
        m_objectsToRender.insert( renderable );
    }
    else
    {
        std::lock_guard<std::mutex> lockGuard( m_objectsToRenderMtx );
        auto it = m_objectsToRender.find( renderable );
        CUL::Assert::simple( it == m_objectsToRender.end(), "Trying to add already added object." );
        m_objectsToRender.insert( renderable );
    }
}

void IGameEngine::removeObjectToRender( IRenderable* renderable )
{
    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        auto it = m_objectsToRender.find( renderable );
        CUL::Assert::simple( it == m_objectsToRender.end(), "Trying to remove already removed object." );
        m_objectsToRender.erase( renderable );
    }
    else
    {
        std::lock_guard<std::mutex> lockGuard( m_objectsToRenderMtx );
        auto it = m_objectsToRender.find( renderable );
        CUL::Assert::simple( it == m_objectsToRender.end(), "Trying to remove already removed object." );
        m_objectsToRender.erase( renderable );
    }
}

IGameEngine::~IGameEngine()
{
}