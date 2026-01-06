#include "gameengine/IGameEngine.hpp"
#include "GameEngineConcrete.hpp"
#include "gameengine/Primitives/Anchor.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Components/TransformComponent.hpp"

#include "gameengine/Primitives/Line.hpp"
#include "gameengine/Primitives/Triangle.hpp"
#include "gameengine/Primitives/Quad.hpp"

#include "gameengine/Cube.hpp"
#include "gameengine/Render/PointLight.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/EngineParams.hpp"
#include "gameengine/VertexBuffer.hpp"
#include "gameengine/EditableTexture.h"
#include "gameengine/Windowing/IWindow.hpp"
#include "gameengine/Windowing/WinData.hpp"

#include "gameengine/ISDL2Wrapper.hpp"
#include "Render/TextureFrameBufferOpenGL.hpp"

#include "CUL/Filesystem/FileFactory.hpp"
#include "CUL/Log/ILogger.hpp"

using namespace LOGLW;

IGameEngine* IGameEngine::s_instance = nullptr;

IGameEngine::IGameEngine()
{
}

IGameEngine* IGameEngine::createGameEngine( LOGLW::ISDL2Wrapper* sdl2w, bool legacy )
{
    s_instance = new GameEngineConcrete( sdl2w, legacy );
    return s_instance;
}

IGameEngine* IGameEngine::createGameEngine( const EngineParams& engineParam )
{
    LOGLW::WinData windowData;
    windowData = engineParam.WinDataVal;

    auto sdlWrap = LOGLW::ISDL2Wrapper::createSDL2Wrapper();
    sdlWrap->init( windowData, engineParam.ConfigPath );

    s_instance = new GameEngineConcrete( sdlWrap, engineParam.legacy );
    return s_instance;
}

void IGameEngine::initialize()
{
    m_camera = std::make_unique<Camera>();

    {
        std::lock_guard<std::mutex> lock( m_initTasksMtx );
        while( !m_initTasks.empty() )
        {
            auto task = m_initTasks.top();
            m_initTasks.pop();
            task();
        }
    }

    IWindow* mainWindow = getMainWindow();

    if( mainWindow->getCurrentRendererType() == RenderTypes::RendererType::OPENGL_MODERN )
    {
        const WinSize winSize = mainWindow->getSize();
        m_frameBufferTexture = std::make_unique<TextureFrameBufferOpenGL>( *getDevice(), winSize.W, winSize.H );
    }
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
    auto sprite = new Sprite( nullptr, false );

    // sprite->LoadImage( path, m_imageLoader );

    addObjectToRender( sprite );

    return sprite;
}

EditableTexture* IGameEngine::createEditableTexture( uint16_t width, uint16_t height )
{
    auto et = new EditableTexture( &getCamera(), getCul(), false );
    et->create( width, height );

    return et;
}

VertexBuffer* IGameEngine::createVBO( const VertexData& vertexData )
{
    VertexBuffer* result = new VertexBuffer( vertexData );

    return result;
}

Line* IGameEngine::createLine( IObject* parent, bool forceLegacy )
{
    Line* line = new Line( getCamera(), parent, forceLegacy );
    return line;
}

Triangle* IGameEngine::createTriangle( IObject* parent, bool forceLegacy )
{
    Triangle* result = new Triangle( getCamera(), parent, forceLegacy );

    return result;
}

Quad* IGameEngine::createQuad( IObject* parent, bool forceLegacy )
{
    Quad* result = new Quad( getCamera(), parent, forceLegacy );

    return result;
}

PointLight* IGameEngine::createPointLight( IObject* parent, bool /*forceLegacy*/ )
{
    PointLight* result = new PointLight( parent );

    return result;
}

Anchor* IGameEngine::createAnchor( IObject* parent, bool forceLegacy )
{
    return new Anchor( getCamera(), parent, forceLegacy );
}

void IGameEngine::addGuiTask( std::function<void( void )> task )
{
    std::lock_guard<std::mutex> lockSection( m_guiTasksMtx );
    m_guiTasks.push( task );
}

void IGameEngine::toggleDrawDebugInfo( bool inEnableDebugInfoDraw )
{
    m_drawDebugInfo = inEnableDebugInfoDraw;
}

bool IGameEngine::getDrawDebugInfo()
{
    return m_drawDebugInfo;
}

VertexArray* IGameEngine::createVAO( const CUL::String& name )
{
    auto result = new VertexArray();
    if( name.empty() == false )
    {
        result->setName( name );
    }
    return result;
}

Cube* IGameEngine::createCube( bool forceLegacy )
{
    Cube* result = new Cube( &getCamera(), forceLegacy );
    result->setName( "Default cube" );
    return result;
}

void IGameEngine::drawOrigin( bool enable )
{
    if( enable )
    {
        if( m_axis[0] == nullptr )
        {
            static float originLinesLength = 4096.f;
            // X
            Line* lineX = createLine( nullptr, false );
            lineX->setColor( ColorE::RED );
            lineX->setLength( originLinesLength );
            lineX->setName( "Line::x" );
            m_axis[0] = lineX;

            CUL::MATH::Rotation rotation;

            // Y
            Line* lineY = createLine( nullptr, false );
            lineY->setColor( ColorE::GREEN );
            rotation.Roll.setValue( 90, CUL::MATH::Angle::Type::DEGREE );
            lineY->getTransform()->setRotationToParent( rotation );
            lineY->setLength( originLinesLength );
            lineY->setName( "Line::y" );
            m_axis[1] = lineY;

            rotation.Roll.setValue( 0, CUL::MATH::Angle::Type::DEGREE );
            rotation.Yaw.setValue( 90, CUL::MATH::Angle::Type::DEGREE );
            // Z
            Line* lineZ = createLine( nullptr, false );
            lineZ->setColor( ColorE::BLUE );
            lineZ->getTransform()->setRotationToParent( rotation );
            lineZ->setLength( originLinesLength );
            lineZ->setName( "Line::z" );
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

ShaderProgram* IGameEngine::createProgram()
{
    ShaderProgram* result = new ShaderProgram();
    m_shadersPrograms.insert( std::make_pair( result, std::unique_ptr<ShaderProgram>( result ) ) );
    return result;
}

void IGameEngine::removeProgram( ShaderProgram* program )
{
    auto it = m_shadersPrograms.find( program );
    if( it != m_shadersPrograms.end() )
    {
        m_shadersPrograms.erase( it );
    }
}

void IGameEngine::addObjectToRender( IRenderable* renderable )
{
    const auto threadName = CUL::CULInterface::getInstance()->getThreadUtils().getThreadName();

    constexpr std::size_t bufferSize{ 512 };
    char buffer[bufferSize];
    snprintf( buffer, bufferSize, "IGameEngine::addObjectToRender: %p [%s]", renderable, threadName.c_str() );

    CUL::LOG::ILogger::getInstance().logVariable( CUL::LOG::Severity::Info, buffer );

    if( getDevice() && CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
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
    const auto threadName = CUL::CULInterface::getInstance()->getThreadUtils().getThreadName();
    CUL::LOG::ILogger::getInstance().logVariable( CUL::LOG::Severity::Info, "IGameEngine::removeObjectToRender: %p [%s]", renderable,
                                                  threadName.c_str() );

    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
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

        if( it == m_objectsToRender.end() )
        {
            CUL::Assert::check( false, "Trying to remove already removed object: %p.", renderable );
        }

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

ShaderProgram* IGameEngine::createShader( const String& /*path*/, const String& /*source*/ )
{
    //ShaderProgram* result = findShader( path );

    //if( result )
    //{
    //    return result;
    //}

    //auto shaderFile = getCul()->getFF()->createRegularFileRawPtr( path );
    //if( !source.empty() )
    //{
    //    shaderFile->loadFromString( source );
    //}

    //throw std::logic_error( "Method not implemented" );
    //// result = new ShaderProgram( *this, shaderFile );
    //m_shaders[path] = result;
    //return result;
    CUL::Assert::check( false, "Method not implemented" );
    return nullptr;
}

void IGameEngine::removeShader( ShaderProgram* /*shader*/ )
{
    // removeShader( shader->getPath() );
    throw std::logic_error( "Method not implemented" );
}

void IGameEngine::removeShader( const String& path )
{
    auto shaderIt = m_shaders.find( path );
    if( shaderIt != m_shaders.end() )
    {
        ShaderProgram* shader = shaderIt->second;
        delete shader;
        m_shaders.erase( shaderIt );
    }
}

const ITextureFrameBuffer* IGameEngine::getFrameBuffer() const
{
    return m_frameBufferTexture.get();
}

ITextureFrameBuffer* IGameEngine::getFrameBuffer()
{
    return m_frameBufferTexture.get();
}

ShaderProgram* IGameEngine::findShader( const String& path ) const
{
    auto shaderIt = m_shaders.find( path );

    if( shaderIt != m_shaders.end() )
    {
        return shaderIt->second;
    }

    return nullptr;
}

void IGameEngine::addPreRenderTask( const std::function<void( void )>& task )
{
    std::lock_guard<std::mutex> lock( m_preRenderTasksMtx );
    m_preRenderTasks.push( task );
}

void IGameEngine::addPostRenderTask( const std::function<void( void )>& task )
{
    std::lock_guard<std::mutex> lock( m_postRenderTasksMtx );
    m_postRenderTasks.push( task );
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