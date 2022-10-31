#include "GameEngineConcrete.hpp"

#include "gameengine/Camera.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Primitives/Triangle.hpp"

#include "Primitives/LineImpl.hpp"
#include "Primitives/PointImpl.hpp"
#include "Primitives/QuadImpl.hpp"
#include "Primitives/QuadImplLegacy.hpp"
#include "LOGLWAdditionalDeps/ImportImgui.hpp"
#include "ObjLoader.hpp"
#include "gameengine/Sprite.hpp"
#include "TextureConcrete.hpp"
#include "DeviceOpenGL.hpp"

#include "SDL2Wrapper/ISDL2Wrapper.hpp"
#include "SDL2Wrapper/IWindow.hpp"
#include "SDL2Wrapper/Input/MouseData.hpp"
#include "SDL2Wrapper/WindowData.hpp"

#include "CUL/Filesystem/FileFactory.hpp"
#include "CUL/GenericUtils/ConsoleUtilities.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"
#include "CUL/ITimer.hpp"
#include "CUL/JSON/INode.hpp"
#include "CUL/STL_IMPORTS/STD_iostream.hpp"
#include "CUL/STL_IMPORTS/STD_condition_variable.hpp"

#undef LoadImage
using namespace LOGLW;

GameEngineConcrete::GameEngineConcrete( SDL2W::ISDL2Wrapper* sdl2w, bool )
    : m_sdlW( sdl2w ),
      m_activeWindow( sdl2w->getMainWindow() ),
      m_cul( sdl2w->getCul() ),
      m_logger( sdl2w->getCul()->getLogger() ),
      m_frameTimer( CUL::TimerFactory::getChronoTimer() )
{
    CUL::Assert::simple( nullptr != sdl2w, "NO SDL WRAPPER." );
    CUL::Assert::simple( nullptr != m_activeWindow, "NO WINDOW." );
    CUL::Assert::simple( nullptr != m_logger, "NO LOGGER." );

    //int firstTextureUnit = GL_TEXTURE0;
    //CUL::Assert::simple( (int)ETextureUnitIndex::UNIT_0 == firstTextureUnit, "Wrong enum map." );

    static_assert( (int)ETextureUnitIndex::UNIT_0 == GL_TEXTURE0, "Incorrect texture unit mapping." );

    loadFromConfig();
    m_imageLoader = m_cul->getImageLoader();

    bool forceLegacy = false;

    const auto config = m_sdlW->getConfig();
    if( config )
    {
        forceLegacy = config->getValue( "OPENGL_FORCE_LEGACY" ).toBool();
    }

    m_oglUtility = new DeviceOpenGL( sdl2w->getCul(), forceLegacy );
    registerObjectForUtility();

    m_renderersVersions["OpenGL"] = m_oglUtility->getVersion();
}

void GameEngineConcrete::registerObjectForUtility()
{
    IUtilityUser::useUtility( m_oglUtility );
}

void GameEngineConcrete::loadFromConfig()
{
    auto config = m_sdlW->getConfig();
    if( config )
    {
        const auto& drawDebug = m_sdlW->getConfig()->getValue( "DRAW_DEBUG" );
        drawDebugInfo( drawDebug == "true" );
    }
}

void GameEngineConcrete::startRenderingLoop()
{
    m_logger->log( "GameEngineConcrete::startRenderingLoop()..." );
    m_renderingLoopThread = std::thread( &GameEngineConcrete::mainThread, this );
    m_logger->log( "GameEngineConcrete::startRenderingLoop() Done." );
}

void GameEngineConcrete::stopRenderingLoop()
{
    m_logger->log( "GameEngineConcrete::stopRenderingLoop()..." );

    m_runRenderLoop = false;

    if( m_taskLoopThread.joinable() )
    {
        m_taskLoopThread.join();
    }

    if( m_renderingLoopThread.joinable() )
    {
        m_renderingLoopThread.join();
    }
    m_logger->log( "GameEngineConcrete::stopRenderingLoop() Done." );
}

void GameEngineConcrete::onInitialize( const EmptyFunctionCallback& callback )
{
    CUL::Assert::simple( m_hasBeenInitialized == false, "Wrapper already initialized, no need in defining " );
    m_onInitializeCallback = callback;
}

void GameEngineConcrete::beforeFrame( const EmptyFunctionCallback& callback )
{
    m_onBeforeFrame = callback;
}

IImageLoader* GameEngineConcrete::getImageLoader()
{
    return m_imageLoader;
}

CUL::LOG::ILogger* GameEngineConcrete::getLoger()
{
    return m_logger;
}

IRenderDevice* GameEngineConcrete::getDevice()
{
    return m_oglUtility;
}

const Viewport& GameEngineConcrete::getViewport() const
{
    return m_viewport;
}

VertexBuffer* GameEngineConcrete::createVBO( std::vector<float>& )
{
    VertexBuffer* result = nullptr;
    return result;
}

const ContextInfo& GameEngineConcrete::getContext() const
{
    return m_glContext;
}

IObject* GameEngineConcrete::createFromFile( const String& path )
{
    const CUL::FS::Path filePath( path );
    if( ".json" == filePath.getExtension() )
    {
        const auto file = m_cul->getFF()->createJSONFileRawPtr( path );
        file->load();
        return createFromFile( file );
    }
    else if( ".obj" == filePath.getExtension() )
    {
        auto objData = ObjLoader::loadObj( path );

        const auto verticesSize = objData->attrib.vertices.size();
        for( size_t i = 0; i < verticesSize; ++i )
        {
            // val = objData->attrib.vertices[i];
            // auto dsadas = 0;
        }

    }
    return nullptr;
}

IObject* GameEngineConcrete::createFromFile( CUL::JSON::IJSONFile* file )
{
    auto root = file->getRoot();
    const auto nameNode = root->findChild( "name" );
    CUL::Assert::simple( nameNode->getType() == CUL::JSON::ElementType::STRING, "Wrong JSON definition: type of name value." );
    if( "default triangle" == nameNode->getString() )
    {
        return createTriangle( root->findChild( "vertices" ) );
    }
    return nullptr;
}

IObject* GameEngineConcrete::createFromFile( IFile* file )
{
    const auto& fileExtension = file->getPath().getExtension();
    if( ".json" == fileExtension )
    {
        const auto jsonFile = static_cast<const CUL::JSON::IJSONFile*>( file );
        const auto root = jsonFile->getRoot();
        const auto name = root->findChild( "name" );
        const auto valueType = name->getType();
        CUL::Assert::simple( valueType == CUL::JSON::ElementType::STRING );

        auto nameVal = name->getString();
    }
    return nullptr;
}

IObject* GameEngineConcrete::createTriangle( CUL::JSON::INode* /*jNode*/ )
{
    IObject* result = nullptr;

    return result;
}

Triangle* GameEngineConcrete::createTriangle( const TriangleData& data, const ColorS& color )
{
    Triangle* triangle = new Triangle( getCamera(), *this, nullptr );
    triangle->setValues( data );
    triangle->setColor( color );
    return triangle;
}

IQuad* GameEngineConcrete::createQuad( const QuadData& data, bool, const ColorS& color )
{
    IQuad* quad = nullptr;
    if( m_oglUtility->isLegacy() )
    {
        m_logger->log( "GameEngineConcrete::createQuad - legacy." );
        quad = new QuadImplLegacy( this );
    }
    else
    {
        m_logger->log( "GameEngineConcrete::createQuad - modern." );
        quad = new QuadImpl( this );
    }
    quad->setValues( data );
    quad->setColor( color );

    return quad;
}

IPoint* GameEngineConcrete::createPoint( const Point& position, const ColorS& color )
{
    auto result = new PointImpl( this );
    result->setColor( color );
    result->getTransform()->setWorldPosition( position );
    addObjectToRender( result );

    return result;
}

Sprite* GameEngineConcrete::createSprite( const String& path, bool )
{
    auto sprite = new Sprite( &getCamera(), getCul(), this );

    CUL::FS::Path fsPath = path;
    CUL::Assert::simple( fsPath.exists(), "File " + path + " does not exist.", m_logger );

    sprite->LoadImage( path, m_imageLoader );

    return sprite;
}

Sprite* GameEngineConcrete::createSprite( unsigned* data, unsigned width, unsigned height, bool )
{
    auto sprite = new Sprite( &getCamera(), getCul(), this );
    auto textureId = m_oglUtility->generateTexture();
    sprite->LoadImage( (CUL::Graphics::DataType*)data, width, height, m_imageLoader, textureId );
    m_oglUtility->bindTexture( textureId );

    const auto& ii = sprite->getImageInfo();

    TextureInfo td;
    td.pixelFormat = CUL::Graphics::PixelFormat::RGBA;
    td.size = ii.size;
    td.data = sprite->getData();
    m_oglUtility->setTextureData( textureId, td );

    m_oglUtility->setTextureParameter( textureId, TextureParameters::MAG_FILTER, TextureFilterType::LINEAR );
    m_oglUtility->setTextureParameter( textureId, TextureParameters::MIN_FILTER, TextureFilterType::LINEAR );

    m_oglUtility->bindTexture( 0 );

    return sprite;
}

void GameEngineConcrete::removeObject( IObject* object )
{
    if( object )
    {
        removeObjectToRender( object );
    }
}

void GameEngineConcrete::mainThread()
{
    getCul()->getThreadUtils().setCurrentThreadName( "RenderThread" );

    initialize();

    m_logger->log( "GameEngineConcrete::start task thread()..." );
    m_taskLoopThread = std::thread( &GameEngineConcrete::taskThread, this );
    m_logger->log( "GameEngineConcrete::start task thread()... Done." );

    renderLoop();

    if( m_debugDrawInitialized )
    {
        if( m_oglUtility->getIsEmbeddedSystems() )
        {
            ImGui_ImplOpenGL3_Shutdown();
        }
        else
        {
            ImGui_ImplOpenGL2_Shutdown();
        }
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    release();
}

void GameEngineConcrete::addRenderThreadTask( const std::function<void( void )>& task )
{
    std::lock_guard<std::mutex> lock( m_taskMutex );
    m_tasks.push( task );
}

void GameEngineConcrete::taskThread()
{
    while( m_runRenderLoop )
    {
        /* code */
    }
}

void GameEngineConcrete::renderLoop()
{
    while( m_runRenderLoop )
    {
        m_frameTimer->start();

        {
            std::lock_guard<std::mutex> lock( m_taskMutex );
            while( !m_tasks.empty() )
            {
                auto task = m_tasks.top();
                m_tasks.pop();
                task();
            }
        }

        renderFrame();

        CUL::ITimer::sleepMicroSeconds( m_frameSleepUs );
        m_frameTimer->stop();

        m_currentFrameLengthUs = (int)m_frameTimer->getElapsed().getUs();

        if( m_everyX == m_everyXMax )
        {
            calculateNextFrameLengths();
            calculateFrameWait();
            m_everyX = 0;
        }
        else
        {
            ++m_everyX;
        }
    }

    size_t objectToRenderLeft = m_objectsToRender.size();
    while( objectToRenderLeft != 0 )
    {
        auto it = m_objectsToRender.begin();
        IRenderable* renderable = *it;
        delete renderable;
        objectToRenderLeft = m_objectsToRender.size();
    }

    releaseResources();
}

void GameEngineConcrete::initDebugInfo()
{
    if( !m_debugDrawInitialized )
    {
        IMGUI_CHECKVERSION();
        auto imguiContext = ImGui::CreateContext();
        setGuiContext( imguiContext );
        ImGui::StyleColorsDark();

        ImGui_ImplSDL2_InitForOpenGL( *m_activeWindow, getContext().glContext );

        if( getDevice()->getIsEmbeddedSystems() )
        {
            ImGui_ImplOpenGL3_Init();
        }
        else
        {
            ImGui_ImplOpenGL2_Init();
        }

        m_debugDrawInitialized = true;
    }
}

void GameEngineConcrete::initialize()
{
    IGameEngine::initialize();

    m_logger->log( "GameEngineConcrete::initialize()..." );

    m_glContext = m_oglUtility->initContextVersion( m_activeWindow );
    m_logger->log( "GameEngineConcrete::initialize(), OpenGL version:" );
    m_logger->log( m_glContext.glVersion );

    m_sdlW->registerSDLEventObserver( this );

    m_oglUtility->setProjectionAndModelToIdentity();
    m_oglUtility->setTexuring( true );

    const auto& winSize = m_activeWindow->getSize();
    setupProjectionData( winSize.getWidth(), winSize.getHeight() );

    m_viewport.pos.setXY( 0, 0 );
    m_viewport.size.setSize( winSize.getWidth(), winSize.getHeight() );

    m_logger->log( "Current viewport:" );
    m_logger->log( "\n" + m_viewport.serialize( 0 ) );

    m_backgroundColor.setAlphaF( 0.0 );
    setBackgroundColor( m_backgroundColor );

    showExtensions();

    calculateFrameWait();
    /*
    m_projectionData.m_depthTest.setOnChange(
        [this]()
        {
            m_projectionChanged = true;
        } );

    m_projectionData.setOnChange(
        [this]()
        {
            m_projectionChanged = true;
        } );

    m_isPerspective.setOnChange(
        [this]()
        {
            if( m_isPerspective )
            {
                m_projectionData.m_projectionType = ProjectionType::PERSPECTIVE;
            }
            else
            {
                m_projectionData.m_projectionType = ProjectionType::ORTO;
            }

            m_projectionChanged = true;
        } );*/

    // m_oglUtility->setBackfaceCUll(  );
    // m_oglUtility->setDepthTest( true );

    getDevice()->toggleDebugOutput( true );

    m_hasBeenInitialized = true;
    m_logger->log( "GameEngineConcrete::initialize() Done." );
}

void GameEngineConcrete::showExtensions()
{
    auto extensionList = m_oglUtility->listExtensions();
    for( const auto& extension : extensionList )
    {
        m_logger->log( "Extension: " + extension );
    }

    m_logger->log( "Extension count: " + std::to_string( extensionList.size() ) );
}

void GameEngineConcrete::setupProjectionData( uint16_t width, uint16_t height )
{
    Camera projectionData;
    projectionData.setSize( { width, height } );
    projectionData.setEyePos( glm::vec3( 0.0f, 0.0f, 220.0f ) );
    projectionData.setCenter( glm::vec3( 0.f, 0.f, 0.0f ) );
    projectionData.setUp( glm::vec3( 0.0f, 1.0f, 0.0f ) );
    setProjection( projectionData );
}

void GameEngineConcrete::renderFrame()
{
    if( m_userInitialized == false )
    {
        if( m_onInitializeCallback )
        {
            m_onInitializeCallback();
            m_userInitialized = true;
        }
    }

    if( m_clearEveryFrame )
    {
        m_oglUtility->clearColorAndDepthBuffer();
    }

    m_oglUtility->setDepthTest( getCamera().getDepthTestIsEnabled() );

    if( getCamera().getProjectionChanged() )
    {
        changeProjectionType();
        getCamera().toggleProjectionChanged( false );
    }

    if( m_onBeforeFrame )
    {
        m_onBeforeFrame();
    }

    if( m_viewportChanged )
    {
        m_oglUtility->setViewport( m_viewport );
        m_viewportChanged = false;
    }

    executeTasks();
    renderObjects();

    if( !m_oglUtility->isLegacy() )
    {
        m_oglUtility->bindBuffer( BufferTypes::ARRAY_BUFFER, 0u );
        m_oglUtility->bindBuffer( BufferTypes::VERTEX_ARRAY, 0u );
    }

    if( m_debugDrawInitialized && m_enableDebugDraw )
    {
        renderInfo();
    }

    refreshBuffers();
}

void GameEngineConcrete::calculateNextFrameLengths()
{
    m_usDelta = ( m_targetFrameLengthUs - m_currentFrameLengthUs ) / 200;
    m_frameSleepUs += m_usDelta;
    m_frameSleepUs = std::max( m_frameSleepUs.getVal(), 0 );

    if( m_usDelta < 0 )
    {
        m_usDelta = 0;
    }
}

#if _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4061 )
#endif
void GameEngineConcrete::renderInfo()
{
    const auto& winSize = m_activeWindow->getSize();

    if( m_oglUtility->getIsEmbeddedSystems() )
    {
        ImGui_ImplOpenGL3_NewFrame();
    }
    else
    {
        ImGui_ImplOpenGL2_NewFrame();
    }

    ImGui_ImplSDL2_NewFrame( *m_activeWindow );

    ImGui::NewFrame();

    String name = "INFO LOG";
    ImGui::Begin( name.cStr(), nullptr,
                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar );
    ImGui::SetWindowPos( { 0, 0 } );
    ImGui::SetWindowSize( { (float)winSize.getWidth() * 0.2f, (float)winSize.getHeight() * 1.f } );

    ImGui::Text( "Legacy: %s", getDevice()->isLegacy() ? "true" : "false" );
    ImGui::Text( "Renderer: %s", m_activeWindow->getRenderName().cStr() );

    float gputTotal = getGPUTotalAvailableMemoryKb();
    gputTotal /= 1024;

    float gpuCurrent = getGPUCurrentAvailableMemoryKb();
    gpuCurrent /= 1024;

    ImGui::Text( "GPU USAGE:" );
    const CUL::String val = CUL::String( (int)gpuCurrent ) + CUL::String( "MB / " ) + CUL::String( (int)gputTotal ) + CUL::String( "MB" );
    ImGui::ProgressBar( gpuCurrent / gputTotal, ImVec2( 0.f, 0.f ), val.cStr() );

    auto res = false;
    {
        static bool isPerspective = getCamera().getProjectionType() == LOGLW::ProjectionType::PERSPECTIVE;
        res = ImGui::Checkbox( "Projection is Perspective", &isPerspective );
        if( res )
        {
            getCamera().setProjectionType( isPerspective ? LOGLW::ProjectionType::PERSPECTIVE : LOGLW::ProjectionType::ORTO );
            getCamera().toggleProjectionChanged( true );
        }
    }

    // ImGui::Checkbox( "Depth test", &m_projectionData.m_depthTest.getRef() );
    // m_projectionData.m_depthTest.runIfChanged();

    ImGui::Text( "Aspect Ratio: %f", getCamera().getAspectRatio() );
    ImGui::Text( "FOV-Y: %f", getCamera().getFov() );

    CUL::Graphics::Pos3Dd centerPos = getCamera().getCenter();
    String text = "Target:" + centerPos.serialize( 0 );
    ImGui::Text( "%s", text.cStr() );

    CUL::Graphics::Pos3Dd eyePos = getCamera().getEye();
    text = "Eye:" + eyePos.serialize( 0 );
    ImGui::Text( "%s", text.cStr() );

    CUL::Graphics::Pos3Dd upPos = getCamera().getUp();
    text = "Up:" + upPos.serialize( 0 );
    ImGui::Text( "%s", text.cStr() );

    const auto& mData = m_sdlW->getMouseData();
    text = "Mouse = ( " + String( mData.getX() ) + ", " + String( mData.getY() ) + " )";
    ImGui::Text( "%s", text.cStr() );

    {
        static float zNear = getCamera().getZnear();
        res = ImGui::SliderFloat( "Z-near", &zNear, -4.f, 8.f );
        if( res )
        {
            getCamera().setZNear( zNear );
        }
    }

    {
        static float zFar = getCamera().getZfar();
        res = ImGui::SliderFloat( "Z-far", &zFar, 0.f, 64.f );
        if( res )
        {
            getCamera().setZfar( zFar );
        }
    }

    {
        static glm::vec3 eye = getCamera().getEye();
        res = ImGui::SliderFloat( "Eye-Z", &eye.z, -512.f, 1024.0f );
        if( res )
        {
            getCamera().setEyePos( eye );
        }
    }

    static glm::vec3 centerCamera = getCamera().getCenter();
    res = ImGui::SliderFloat( "Center-Z", &centerCamera.z, -64.0f, 255.0f );
    if( res )
    {
        getCamera().setCenter( centerCamera );
        getCamera().toggleProjectionChanged( true );
    }

    text = "Left: " + String( getCamera().getLeft() );
    ImGui::Text( "%s", text.cStr() );

    text = "Right: " + String( getCamera().getRight() );
    ImGui::Text( "%s", text.cStr() );

    text = "Top: " + String( getCamera().getTop() );
    ImGui::Text( "%s", text.cStr() );

    text = "Bottom: " + String( getCamera().getBottom() );
    ImGui::Text( "%s", text.cStr() );

    for( const auto& pair : m_debugValues )
    {
        if( pair.second.type == DebugType::TEXT )
        {
            const size_t id = pair.second.value.index();
            switch( id )
            {
                case 0:
                    ImGui::Text( pair.second.text.cStr(), *(const char*)std::get<String*>( pair.second.value ) );
                    break;
                case 1:
                    ImGui::Text( pair.second.text.cStr(), *std::get<float*>( pair.second.value ) );
                    break;
                case 2:
                    ImGui::Text( pair.second.text.cStr(), *std::get<int*>( pair.second.value ) );
                    break;
                default:
                    break;
            }
        }
        else if( pair.second.type == DebugType::SLIDER )
        {
            const size_t id = pair.second.value.index();
            bool changed = false;
            switch( id )
            {
                case 0:
                    // ImGui::Text( pair.second.text.cStr(),  );
                    // const auto changed = ImGui::SliderFloat(
                    // pair.second.text.cStr(), *std::get<String*>(
                    // pair.second.value ) 0.0f, 192.0f );
                    break;
                case 1:
                    changed = ImGui::SliderFloat( pair.second.text.cStr(), std::get<float*>( pair.second.value ), pair.second.min,
                                                  pair.second.max );
                    break;
                case 2:
                    changed = ImGui::SliderInt( pair.second.text.cStr(), std::get<int*>( pair.second.value ), (int)pair.second.min,
                                                (int)pair.second.max );
                    break;
                default:
                    break;
            }

            if( changed && pair.second.onChange )
            {
                pair.second.onChange();
            }
        }
    }

    ImGui::Text( "FrameTime: %4.2f ms", 1000.f / ImGui::GetIO().Framerate );
    ImGui::Text( "FPS: %4.2f", m_activeWindow->getFpsCounter()->getCurrentFps() );

    ImGui::Text( "m_currentFrameLengthUs: %d", m_currentFrameLengthUs.getValCopy() );
    ImGui::Text( "m_targetFrameLengthUs: %d", m_targetFrameLengthUs.getValCopy() );
    ImGui::Text( "m_frameSleepUs: %d", m_frameSleepUs.getValCopy() );
    ImGui::Text( "m_usDelta: %d", m_usDelta );

    ImGui::End();

    {
        std::lock_guard<std::mutex> locker( m_guiTasksMtx );
        while( false == m_guiTasks.empty() )
        {
            auto task = m_guiTasks.front();
            task();
            m_guiTasks.pop();
        }
    }

    guiFrameDelegate.execute();

    ImGui::Render();

    if( m_oglUtility->getIsEmbeddedSystems() )
    {
        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
    }
    else
    {
        ImGui_ImplOpenGL2_RenderDrawData( ImGui::GetDrawData() );
    }
}
#if _MSC_VER
#pragma warning( pop )
#endif

// void GameEngineConcrete::setProjectionType( const ProjectionType* type )
//{
//     m_projectionData.m_projectionType = *type;
//     prepareProjection();
//     m_projectionChanged = true;
// }

void GameEngineConcrete::prepareProjection()
{
    // if( ProjectionType::ORTO == m_projectionData.m_projectionType )
    //{
    //    auto eyeCopy = m_projectionData.getEye();
    //    eyeCopy.z = std::max( eyeCopy.z, m_projectionData.getZnear() );
    //    m_projectionData.setZNear( eyeCopy.z );
    //    m_projectionData.setEyePos( eyeCopy );
    //}
    // else if( ProjectionType::PERSPECTIVE == m_projectionData.m_projectionType
    // )
    //{
    //    m_projectionData.setZNear( std::min( m_projectionData.getEye().z,
    //    m_projectionData.getZnear() ) );
    //}
}

void GameEngineConcrete::changeProjectionType()
{
    m_oglUtility->resetMatrixToIdentity( MatrixTypes::PROJECTION );

    if( ProjectionType::ORTO == getCamera().getProjectionType() )
    {
        m_oglUtility->setOrthogonalPerspective( getCamera() );
    }
    else if( ProjectionType::PERSPECTIVE == getCamera().getProjectionType() )
    {
        m_oglUtility->setPerspectiveProjection( getCamera() );
    }
    m_oglUtility->resetMatrixToIdentity( MatrixTypes::MODELVIEW );
    m_oglUtility->lookAt( getCamera() );
    setProjection( getCamera() );
    m_oglUtility->setDepthTest( getCamera().getDepthTestIsEnabled() );
}

void GameEngineConcrete::setEyePos( const glm::vec3& pos )
{
    getCamera().setEyePos( pos );
}

void GameEngineConcrete::executeTasks()
{
    std::lock_guard<std::mutex> locker( m_preRenderTasksMtx );

    while( false == m_preRenderTasks.empty() )
    {
        auto task = m_preRenderTasks.front();
        task->execute();
        m_preRenderTasks.pop();
    }

    while( false == m_preRenderTasksFunction.empty() )
    {
        auto task = m_preRenderTasksFunction.front();
        task();
        m_preRenderTasksFunction.pop();
    }
}

void GameEngineConcrete::renderObjects()
{
    std::lock_guard<std::mutex> lockGuard( m_objectsToRenderMtx );
    for( auto& renderableObject : m_objectsToRender )
    {
        m_oglUtility->useProgram( 0u );
        renderableObject->render();
    }
}

void GameEngineConcrete::refreshBuffers()
{
    if( m_updateBuffers )
    {
        m_activeWindow->updateScreenBuffers();
    }
}

void GameEngineConcrete::setProjection( const Camera& rect )
{
    m_oglUtility->setProjection( rect );
}

void GameEngineConcrete::setViewport( const Viewport& viewport, const bool instant )
{
    if( m_viewport != viewport )
    {
        m_viewport = viewport;
        if( false == instant )
        {
            m_viewportChanged = true;
        }
        else
        {
            m_oglUtility->setViewport( m_viewport );
        }
    }
}

void GameEngineConcrete::setBackgroundColor( const ColorS& color )
{
    m_oglUtility->clearColorTo( color );
    m_backgroundColor = color;
}

void GameEngineConcrete::release()
{
    m_logger->log( "GameEngineConcrete::release()..." );

    m_sdlW->unRegisterSDLEventObserver( this );
    getMainWindow()->destroyContext( m_glContext.glContext );
    m_glContext.glContext = nullptr;

    m_logger->log( "GameEngineConcrete::release()... Done." );
}

void GameEngineConcrete::drawQuad( const bool draw )
{
    m_drawQuad = draw;
}

void GameEngineConcrete::clearModelViewEveryFrame( const bool enable )
{
    m_clearModelView = enable;
}

void GameEngineConcrete::calculateFrameWait()
{
    m_targetFrameLengthUs = 1000000.0f / m_fpsLimit;
}

CUL::GUTILS::IConfigFile* GameEngineConcrete::getConfig()
{
    CUL::Assert::simple( m_sdlW != nullptr, "No proper SDL2 pointer initialized." );
    return m_sdlW->getConfig();
}

void GameEngineConcrete::drawDebugInfo( const bool enable )
{
    m_enableDebugDraw = enable;

    auto currentThreadName = getCul()->getThreadUtils().getCurrentThreadName();

    if( getCul()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        initDebugInfo();
    }
    else
    {
        addRenderThreadTask(
            [this]()
            {
                initDebugInfo();
            } );
    }

    m_sdlW->getMainWindow()->toggleFpsCounter( enable );
}

IDebugOverlay* GameEngineConcrete::getDebugOverlay()
{
    return this;
}

void GameEngineConcrete::handleEvent( const SDL_Event& event )
{
    if( m_debugDrawInitialized )
    {
        ImGui_ImplSDL2_ProcessEvent( &event );
    }
}

unsigned GameEngineConcrete::addSliderValue( const CUL::String& text, float* val, float min, float max,
                                             const std::function<void( void )>& onUpdate )
{
    const unsigned size = (unsigned)m_debugValues.size();
    const auto newId = size + 1u;

    DebugValueRow row;
    row.type = DebugType::SLIDER;
    row.id = newId;
    row.min = min;
    row.max = max;
    row.value = val;
    row.text = text;
    row.onChange = onUpdate;

    m_debugValues[newId] = row;

    return newId;
}

unsigned GameEngineConcrete::addText( const CUL::String& text, float* val )
{
    const unsigned size = (unsigned)m_debugValues.size();
    const auto newId = size + 1u;

    DebugValueRow row;
    row.type = DebugType::TEXT;
    row.id = newId;
    row.value = val;
    row.text = text;

    m_debugValues[newId] = row;

    return newId;
}

void GameEngineConcrete::runEventLoop()
{
    m_sdlW->runEventLoop();
}

void GameEngineConcrete::stopEventLoop()
{
    m_sdlW->stopEventLoop();
}

SDL2W::IWindow* GameEngineConcrete::getMainWindow()
{
    return m_sdlW->getMainWindow();
}

ITextureFactory* GameEngineConcrete::getTextureFactory()
{
    return this;
}

ITexture* GameEngineConcrete::createTexture( const CUL::FS::Path& path, const bool )
{
    // auto image = m_sdlW->getCul()->getImageLoader()->loadImage( path, rgba );
    auto textureConcrete = new TextureConcrete( getDevice(), m_sdlW->getCul()->getImageLoader(), path );
    return textureConcrete;
}

// SDL2W::IMouseObservable
void GameEngineConcrete::addMouseEventCallback( const SDL2W::IMouseObservable::MouseCallback& callback )
{
    m_sdlW->addMouseEventCallback( callback );
}

void GameEngineConcrete::registerMouseEventListener( SDL2W::IMouseObserver* observer )
{
    m_sdlW->registerMouseEventListener( observer );
}

void GameEngineConcrete::unregisterMouseEventListener( SDL2W::IMouseObserver* observer )
{
    m_sdlW->unregisterMouseEventListener( observer );
}

SDL2W::MouseData& GameEngineConcrete::getMouseData()
{
    return m_sdlW->getMouseData();
}

// SDL2W::IKeyboardObservable
void GameEngineConcrete::registerKeyboardEventCallback( const std::function<void( const SDL2W::KeyboardState& keyboardState )>& callback )
{
    m_sdlW->registerKeyboardEventCallback( callback );
}

void GameEngineConcrete::registerKeyboardEventListener( SDL2W::IKeyboardObserver* observer )
{
    m_sdlW->registerKeyboardEventListener( observer );
}

void GameEngineConcrete::unregisterKeyboardEventListener( SDL2W::IKeyboardObserver* observer )
{
    m_sdlW->unregisterKeyboardEventListener( observer );
}

bool GameEngineConcrete::isKeyUp( const String& keyName ) const
{
    return m_sdlW->isKeyUp( keyName );
}

void GameEngineConcrete::registerWindowEventCallback( const SDL2W::WindowCallback& callback )
{
    m_sdlW->registerWindowEventCallback( callback );
}

CUL::CULInterface* GameEngineConcrete::getCul()
{
    return m_sdlW->getCul();
}

void GameEngineConcrete::setFpsLimit( float maxFps )
{
    m_fpsLimit = maxFps;
}

float GameEngineConcrete::getFpsLimit() const
{
    return m_fpsLimit.getVal();
}

GameEngineConcrete::~GameEngineConcrete()
{
    m_logger->log( "GameEngineConcrete::~GameEngineConcrete()..." );

    m_logger->log( "GameEngineConcrete::~GameEngineConcrete() Done." );
}
