#include "gameengine/IGameEngine.hpp"
#include "GameEngineConcrete.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Components/TransformComponent.hpp"

#include "gameengine/Primitives/Line.hpp"
#include "gameengine/Primitives/Triangle.hpp"
#include "gameengine/Primitives/Quad.hpp"

#include "gameengine/Cube.hpp"
#include "gameengine/Program.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/EngineParams.hpp"

#include "SDL2Wrapper/WindowData.hpp"
#include "SDL2Wrapper/ISDL2Wrapper.hpp"
#include "SDL2Wrapper/IWindow.hpp"

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

IGameEngine* IGameEngine::createGameEngine( const EngineParams& engineParam  )
{
    SDL2W::WindowData windowData;
    windowData.name = engineParam.winName;
    windowData.pos = engineParam.windowPosition;
    windowData.currentRes = engineParam.winSize;
    windowData.rendererName = engineParam.rendererName;

    auto sdlWrap = SDL2W::ISDL2Wrapper::createSDL2Wrapper();
    sdlWrap->init( windowData, engineParam.configPath );

    s_instance = new GameEngineConcrete( sdlWrap, engineParam.legacy );
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


Line* IGameEngine::createLine( IObject* parent )
{
    Line* line = new Line( getCamera(), *this, parent );
    return line;
}

Triangle* IGameEngine::createTriangle( IObject* parent )
{
    Triangle* result = new Triangle( getCamera(), *this, parent );

    return result;
}


Quad* IGameEngine::createQuad( IObject* parent )
{
    Quad* result = new Quad( getCamera(), *this, parent );

    return result;
}

void IGameEngine::addGuiTask( std::function<void(void)> task )
{
    std::lock_guard<std::mutex> lockSection( m_guiTasksMtx );
    m_guiTasks.push( task );
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

void IGameEngine::drawOrigin( bool enable )
{
    if( enable )
    {
        if( m_axis[0] == nullptr )
        {
            static float originLinesLength = 4096.f;
            // X
            Line* lineX = createLine( nullptr );
            lineX->setColor( ColorE::RED );
            lineX->setLength( originLinesLength );
            m_axis[0] = lineX;

            CUL::MATH::Rotation rotation;

            // Y
            Line* lineY = createLine( nullptr );
            lineY->setColor( ColorE::GREEN );
            rotation.roll.setValue( 90, CUL::MATH::Angle::Type::DEGREE );
            lineY->getTransform()->setWorldRotation( rotation );
            lineY->setLength( originLinesLength );
            m_axis[1] = lineY;

            rotation.roll.setValue( 0, CUL::MATH::Angle::Type::DEGREE );
            rotation.yaw.setValue( 90, CUL::MATH::Angle::Type::DEGREE );
            // Z
            Line* lineZ = createLine( nullptr );
            lineZ->setColor( ColorE::BLUE );
            lineZ->getTransform()->setWorldRotation( rotation );
            lineZ->setLength( originLinesLength );
            m_axis[0] = lineZ;
        }
    }
    else
    {
        if( m_axis[0] == nullptr )
        {
            for( const auto& axis : m_axis )
            {
                removeObject( axis );
            }
        }
    }
}

Program* IGameEngine::getDefaultShader()
{
    Program* result = createProgram();

    if( getMainWindow()->getRenderName().toLowerR().contains( "opengl" ) )
    {
        const std::string vertexShaderSource =
    #include "embedded_shaders/basic_pos.vert"
            ;

        const std::string fragmentShaderSource =
    #include "embedded_shaders/basic_color.frag"
            ;

        auto fragmentShader = createShader( "embedded_shaders/basic_color.frag", fragmentShaderSource );
        auto vertexShader = createShader( "embedded_shaders/basic_pos.vert", vertexShaderSource );

        result->attachShader( vertexShader );
        result->attachShader( fragmentShader );
        result->link();
        result->validate();

        result->enable();
    }
    else
    {
        CUL::Assert::simple( false, "Not implemented yet." );
    }

    return result;
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
    if( getDevice()->getCUL()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
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
    if( getDevice()->getCUL()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
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
    if( getDevice()->getCUL()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
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
    return getDevice()->getGPUTotalAvailableMemoryKb();
}

unsigned IGameEngine::getGPUCurrentAvailableMemoryKb()
{
    return getDevice()->getGPUCurrentAvailableMemoryKb();
}

ImGuiContext* IGameEngine::getGuiContext() const
{
    return m_ImGuiContext;
}

void IGameEngine::setGuiContext( ImGuiContext* const inContext )
{
    m_ImGuiContext = inContext;
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

IObjectFactory* IGameEngine::getObjectFactory()
{
    return this;
}

IGameEngine::~IGameEngine()
{
    releaseResources();

    delete m_renderDevice;
    m_renderDevice = nullptr;
}


void IGameEngine::releaseResources()
{
    m_shaders.clear();
    m_shadersPrograms.clear();
}