#include "gameengine/IGameEngine.hpp"
#include "GameEngineConcrete.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Cube.hpp"
#include "gameengine/Program.hpp"
#include "gameengine/VertexArray.hpp"

#include "SDL2Wrapper/WindowData.hpp"
#include "SDL2Wrapper/ISDL2Wrapper.hpp"

#include "CUL/Filesystem/FileFactory.hpp"

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

IGameEngine* IGameEngine::createGameEngine( bool legacy, const CUL::Graphics::Pos2Di& pos, const SDL2W::WinSize& winSize,
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
    m_camera = std::make_unique<Camera>();
}

Camera& IGameEngine::getCamera()
{
    return *m_camera.get();
}

IGameEngine* IGameEngine::getInstance()
{
    return s_instance;
}

Sprite* IGameEngine::createSprite()
{
    auto sprite = new Sprite( &getCamera(), getCul(), this );

    // sprite->LoadImage( path, m_imageLoader );

    addObjectToRender( sprite );

    return sprite;
}

Quad* IGameEngine::createQuad( IObject* parent )
{
    Quad* result = new Quad( getCamera(), *this, parent );

    return result;
}

VertexArray* IGameEngine::createVAO()
{
    auto result = new VertexArray( *this );
    return result;
}

Cube* IGameEngine::createCube()
{
    return new Cube( &getCamera(), this );
}

Program* IGameEngine::createProgram()
{
    Program* result = new Program( *this );
    m_shadersPrograms.insert( std::make_pair(result, std::unique_ptr<Program>(result)) );
    return result;
}

void IGameEngine::removeProgram( Program* program )
{
    auto it = m_shadersPrograms.find( program );
    if( it != m_shadersPrograms.end() )
    {
        m_shadersPrograms.erase( it );
    }
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
        if( it != m_objectsToRender.end() )
        {
            m_objectsToRender.erase( it );
        }
    }
    else
    {
        std::lock_guard<std::mutex> lockGuard( m_objectsToRenderMtx );
        auto it = m_objectsToRender.find( renderable );
        CUL::Assert::simple( it == m_objectsToRender.end(), "Trying to remove already removed object." );
        m_objectsToRender.erase( renderable );
    }
}

void IGameEngine::toggleGrid( bool enableGrid )
{
    m_gridEnabled = enableGrid;
}

unsigned IGameEngine::getGPUTotalAvailableMemoryKb()
{
    return getUtility()->getGPUTotalAvailableMemoryKb();
}

unsigned IGameEngine::getGPUCurrentAvailableMemoryKb()
{
    return getUtility()->getGPUCurrentAvailableMemoryKb();
}

Shader* IGameEngine::createShader( const String& path, const String& source )
{
    Shader* result = findShader( path );

    if( result )
    {
        return result;
    }

    auto shaderFile = getCul()->getFF()->createRegularFileRawPtr( path );
    if( !source.empty() )
    {
        shaderFile->loadFromString( source );
    }

    result = new Shader( *this, shaderFile );
    m_shaders[path] = result;
    return result;
}

void IGameEngine::removeShader( Shader* shader )
{
    removeShader( shader->getPath() );
}

void IGameEngine::removeShader( const String& path )
{
    auto shaderIt = m_shaders.find( path );
    if( shaderIt != m_shaders.end() )
    {
        Shader* shader = shaderIt->second;
        delete shader;
        m_shaders.erase( shaderIt );
    }
}

Shader* IGameEngine::findShader( const String& path ) const
{
    auto shaderIt = m_shaders.find( path );

    if( shaderIt != m_shaders.end() )
    {
        return shaderIt->second;
    }

    return nullptr;
}

IGameEngine::~IGameEngine()
{
    releaseResources();

    delete m_oglUtility;
    m_oglUtility = nullptr;
}


void IGameEngine::releaseResources()
{
    m_shaders.clear();
    m_shadersPrograms.clear();
}