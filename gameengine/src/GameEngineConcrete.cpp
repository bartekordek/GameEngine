#include "GameEngineConcrete.hpp"

#include "gameengine/Camera.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/ExecuteType.hpp"
#include "gameengine/Primitives/Triangle.hpp"
#include "gameengine/VertexArray.hpp"

#include "Primitives/LineImpl.hpp"
#include "LOGLWAdditionalDeps/ImportImgui.hpp"
#include "gameengine/Sprite.hpp"
#include "TextureConcrete.hpp"
#include "DeviceOpenGL.hpp"
#include "DX12/DeviceDX12.hpp"
#include "DX09/DeviceDX09.hpp"

#include "DebugUtil/DebugSystemBase.hpp"
#include "DebugUtil/DebugSystemParams.hpp"

#include "gameengine/ISDL2Wrapper.hpp"
#include "gameengine/Windowing/IWindow.hpp"
#include "gameengine/Input/MouseData.hpp"
#include "gameengine/Windowing/WinData.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/Render/FrameTimeManager.hpp"

#include "CUL/Filesystem/FileFactory.hpp"
#include "CUL/GenericUtils/ConsoleUtilities.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"
#include "CUL/ITimer.hpp"
#include "CUL/Math/Utils.hpp"
#include "CUL/JSON/INode.hpp"
#include "CUL/IMPORT_tracy.hpp"

#include "CUL/STL_IMPORTS/STD_iostream.hpp"
#include "CUL/STL_IMPORTS/STD_condition_variable.hpp"
#include "CUL/STL_IMPORTS/STD_exception.hpp"

#undef LoadImage
using namespace LOGLW;

GameEngineConcrete::GameEngineConcrete( LOGLW::ISDL2Wrapper* sdl2w, bool ):
    m_sdlW( sdl2w ),
    m_activeWindow( sdl2w->getMainWindow() ),
    m_cul( sdl2w->getCul() ),
    m_logger( sdl2w->getCul()->getLogger() )
{
    CUL::Assert::simple( nullptr != sdl2w, "NO SDL WRAPPER." );
    CUL::Assert::simple( nullptr != m_activeWindow, "NO WINDOW." );
    CUL::Assert::simple( nullptr != m_logger, "NO LOGGER." );

    auto initTask = [this]()
    {
        DebugSystemParams params;
        static_assert( (int)ETextureUnitIndex::UNIT_0 == GL_TEXTURE0, "Incorrect texture unit mapping." );

        m_imageLoader = m_cul->getImageLoader();

        bool forceLegacy = false;

        const auto config = m_sdlW->getConfig();
        if( config )
        {
            const CUL::ThreeState threeState = config->getValue( "OPENGL_FORCE_LEGACY" ).toBool();
            forceLegacy = threeState == CUL::ThreeState::True ? true : false;
        }

        const auto rendererType = m_activeWindow->getCurrentRendererType();

        if( rendererType == LOGLW::RenderTypes::RendererType::DIRECTX_9 )
        {
#if defined( GAME_ENGINE_WINDOWS )
            m_renderDevice = new DeviceDX09();
#else
            CUL::Assert::simple( false, "NOTE IMPLEMENTED." );
#endif
        }
        else if( rendererType == LOGLW::RenderTypes::RendererType::DIRECTX_12 )
        {
#if defined( GAME_ENGINE_WINDOWS )
            m_renderDevice = new DeviceDX12();
#else
            CUL::Assert::simple( false, "NOTE IMPLEMENTED." );
#endif
        }
        else
        {
            m_renderDevice = new DeviceOpenGL( forceLegacy );
            m_renderersVersions["OpenGL"] = m_renderDevice->getVersion();
        }

        loadDebugDraw();
        registerObjectForUtility();

        m_glContext = m_renderDevice->initContextVersion( m_activeWindow );
        m_logger->log( "GameEngineConcrete::initialize(), OpenGL version:" );
        m_logger->log( m_glContext.glVersion );

        m_debugSystem.reset( DebugSystemBase::create( m_renderDevice->getType() ) );

        params.SDLWindow = m_activeWindow->getSDLWindow();
        params.SDL_GL_Context = m_glContext.glContext;

        if( m_debugSystem )
        {
            m_debugSystem->init( params );
            m_debugSystem->addRenderCallback(
                [this]()
                {
                    renderInfo();
                } );
        }

        CUL::ThreeState drawDebugInfo = config->getValue( "DRAW_DEBUG_INFO" ).toBool();

        toggleDrawDebugInfo( ( drawDebugInfo == CUL::ThreeState::True || drawDebugInfo == CUL::ThreeState::Undetermined ) ? true : false );
    };
    {
        std::lock_guard<std::mutex> lock( m_initTasksMtx );
        m_initTasks.push( initTask );
    }

    setFpsLimit( 60.f );
    FrameTimeManager::getInstance().setSamplesCount( 2048 );
}

void GameEngineConcrete::registerObjectForUtility()
{
    IUtilityUser::useUtility( m_renderDevice );
}

void GameEngineConcrete::loadDebugDraw()
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
    return m_renderDevice;
}

const Viewport& GameEngineConcrete::getViewport() const
{
    return m_viewport;
}

const ContextInfo& GameEngineConcrete::getContext() const
{
    return m_glContext;
}

Sprite* GameEngineConcrete::createSprite( const String& path, bool )
{
    auto sprite = new Sprite( *this, nullptr, false );

    CUL::FS::Path fsPath = path;
    CUL::Assert::simple( fsPath.exists(), "File " + path + " does not exist.", m_logger );

    sprite->LoadImage( path, m_imageLoader );

    return sprite;
}

Sprite* GameEngineConcrete::createSprite( unsigned* data, unsigned width, unsigned height, bool )
{
    auto sprite = new Sprite( *this, nullptr, false );
    auto textureId = m_renderDevice->generateTexture();
    sprite->LoadImage( (CUL::Graphics::DataType*)data, width, height, m_imageLoader, textureId );
    m_renderDevice->bindTexture( textureId );

    const auto& ii = sprite->getImageInfo();

    TextureInfo td;
    td.pixelFormat = CUL::Graphics::PixelFormat::RGBA;
    td.size = ii.size;
    td.data = sprite->getData();
    m_renderDevice->setTextureData( textureId, td );

    m_renderDevice->setTextureParameter( textureId, TextureParameters::MAG_FILTER, TextureFilterType::LINEAR );
    m_renderDevice->setTextureParameter( textureId, TextureParameters::MIN_FILTER, TextureFilterType::LINEAR );

    m_renderDevice->bindTexture( 0 );

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
    getCul()->getThreadUtils().setThreadName( "RenderThread" );

    initialize();

    m_logger->log( "GameEngineConcrete::start task thread()..." );
    m_taskLoopThread = std::thread( &GameEngineConcrete::taskThread, this );
    m_logger->log( "GameEngineConcrete::start task thread()... Done." );

    renderLoop();

    m_debugSystem.reset();

    release();
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
    if( m_onInitializeCallback )
    {
        m_onInitializeCallback();
    }

    while( m_runRenderLoop )
    {
        ZoneScoped;
        FrameTimeManager::getInstance().startFrame();
        runPreRenderTasks();
        renderFrame();
        runPostRenderTasks();
        FrameTimeManager::getInstance().endFrame();
        FrameMark;
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

void GameEngineConcrete::initialize()
{
    IGameEngine::initialize();

    m_logger->log( "GameEngineConcrete::initialize()..." );

    const auto& winSize = m_activeWindow->getSize();
    const float aspectRatio = static_cast<float>( winSize.W ) / static_cast<float>( winSize.H );
    getCamera().setAspectRatio( aspectRatio );

    m_sdlW->registerSDLEventObserver( this );

    setupProjectionData( winSize.W, winSize.H );

    m_viewport.pos.setXY( 0, 0 );
    m_viewport.size.setSize( winSize.W, winSize.H );

    m_logger->log( "Current viewport:" );
    m_logger->log( "\n" + m_viewport.serialize( 0 ) );

    m_backgroundColor.setAlphaF( 0.0 );
    setBackgroundColor( m_backgroundColor );

    // showExtensions();

    getDevice()->toggleDebugOutput( true );

    m_hasBeenInitialized = true;
    m_logger->log( "GameEngineConcrete::initialize() Done." );
}

void GameEngineConcrete::showExtensions()
{
    auto extensionList = m_renderDevice->listExtensions();
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
}

void GameEngineConcrete::renderFrame()
{
    ZoneScoped;
    m_renderDevice->prepareFrame();

    if( m_clearEveryFrame )
    {
        m_renderDevice->clearColorAndDepthBuffer();
    }

    m_renderDevice->setDepthTest( getCamera().getDepthTestIsEnabled() );

    if( getCamera().getProjectionChanged() )
    {
        // changeProjectionType();
        // getCamera().toggleProjectionChanged( false );
    }

    if( m_onBeforeFrame )
    {
        m_onBeforeFrame();
    }

    if( m_viewportChanged )
    {
        m_renderDevice->setViewport( m_viewport );
        m_viewportChanged = false;
    }
    renderObjects();

    if( !m_renderDevice->isLegacy() )
    {
        m_renderDevice->bindBuffer( BufferTypes::ARRAY_BUFFER, 0u );
        m_renderDevice->bindBuffer( BufferTypes::VERTEX_ARRAY, 0u );
    }

    if( m_debugSystem.get() && m_enableDebugDraw )
    {
        m_debugSystem->frame();
    }

    finishFrame();
}

void drawObjects( std::set<IObject*>& shownList, IObject* currentObject, const CUL::String& name );

#if _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4061 )
#endif
void GameEngineConcrete::renderInfo()
{
    ZoneScoped;
    float debugInfoWidth{ 0.f };
    float debugInfoHeight{ 0.f };
    if( getDrawDebugInfo() )
    {
        String name = "INFO LOG";
        ImGui::Begin( name.cStr(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar );
        ImGui::SetWindowPos( { 0, 0 } );

        const auto& winSize = m_activeWindow->getSize();
        debugInfoWidth = (float)winSize.W * 0.42f;
        debugInfoWidth = (float)winSize.H * 1.f;
        ImGui::SetNextWindowSize( { debugInfoWidth, debugInfoWidth } );

        ImGui::Text( "Legacy: %s", getDevice()->isLegacy() ? "true" : "false" );
        ImGui::Text( "Renderer: %s", getDevice()->getName().cStr() );

        float gputTotal = getGPUTotalAvailableMemoryKb();
        gputTotal /= 1024;

        float gpuCurrent = getGPUCurrentAvailableMemoryKb();
        gpuCurrent /= 1024;

        ImGui::Text( "GPU USAGE:" );
        const CUL::String val =
            CUL::String( (int)gpuCurrent ) + CUL::String( "MB / " ) + CUL::String( (int)gputTotal ) + CUL::String( "MB" );
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
        String text;
        {
            ZoneScoped;
            ImGui::Text( "Aspect Ratio: %f", getCamera().getAspectRatio() );
            ImGui::Text( "FOV-Y: %f", getCamera().getFov() );

            const auto& mData = m_sdlW->getMouseData();
            text = "Mouse = ( " + String( mData.getX() ) + ", " + String( mData.getY() ) + " )";
            ImGui::Text( "%s", text.cStr() );
        }
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

        constexpr float slidermaxVal = 255.f;
        {
            static glm::vec3 eye = getCamera().getEye();
            res = ImGui::SliderFloat( "Eye-X", &eye.x, -slidermaxVal, slidermaxVal );
            if( res )
            {
                getCamera().setEyePos( eye );
            }
        }

        {
            static glm::vec3 eye = getCamera().getEye();
            res = ImGui::SliderFloat( "Eye-Y", &eye.y, -slidermaxVal, slidermaxVal );
            if( res )
            {
                getCamera().setEyePos( eye );
            }
        }

        {
            static glm::vec3 eye = getCamera().getEye();
            res = ImGui::SliderFloat( "Eye-Z", &eye.z, -slidermaxVal, slidermaxVal );
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

        ImGui::Text( "FPS (Imgui): %4.2f", ImGui::GetIO().Framerate );
        ImGui::Text( "FrameTime: %4.2f ms", 1000.f / ImGui::GetIO().Framerate );

        ImGui::Text( "Average frame ms: %f", FrameTimeManager::getInstance().geAvgFrameTimeMS() );
        ImGui::Text( "Target frame ms: %f", FrameTimeManager::getInstance().getTargetFrameTimeMS() );
        ImGui::Text( "Average FPS: %f", FrameTimeManager::getInstance().getAvgFPS() );

        drawObjectsInfo( debugInfoWidth, debugInfoHeight );
    }
    guiFrameDelegate.execute( debugInfoWidth, debugInfoWidth );
}

#if _MSC_VER
#pragma warning( pop )
#endif

bool GameEngineConcrete::drawObjectsInfo( float& width, float& high )
{
    const auto& winSize = m_activeWindow->getSize();

    if( ImGui::TreeNode( "Objects" ) )
    {
        std::set<IObject*> shownList;

        for( IRenderable* renderable : m_objectsToRender )
        {
            IObject* object = renderable->getObject()->getOuter();
            if( object )
            {
                drawObjects( shownList, object, object->getName() );
            }
        }

        ImGui::TreePop();
    }

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

    return true;
}

void GameEngineConcrete::drawObjects( std::set<IObject*>& shownList, IObject* currentObject, const CUL::String& name )
{
    if( shownList.find( currentObject ) != shownList.end() )
    {
        return;
    }

    CUL::Assert::simple( name.empty() == false, "GameEngineConcrete::drawObjects: object has no name." );

    constexpr std::size_t stringBufferLength{ 32u };
    static char stringBuffer[stringBufferLength];

    if( ImGui::TreeNode( name.cStr() ) )
    {
        ImGui::Text( "Name: %s", name.cStr() );

        Sprite* spriteObject = dynamic_cast<Sprite*>( currentObject );

        if( spriteObject )
        {
            drawSpriteData( spriteObject );
        }

        LOGLW::TransformComponent* transform = currentObject->getTransform();
        if( transform != nullptr && ImGui::TreeNode( "Transform" ) )
        {
            drawTransformData( transform );
            ImGui::TreePop();
        }

        constexpr std::size_t labelBufferLength{ 32u };
        static char labelBuffer[labelBufferLength];

        String fString;

        if( ShaderProgram* shaderProgram = currentObject->getProgram() )
        {
            if( shaderProgram->getIsLinked() )
            {
                if( ImGui::TreeNode( "Uniforms" ) )
                {
                    const std::vector<CUL::String> uniformNames = shaderProgram->getUniformsNames();
                    for( const CUL::String& uniformName : uniformNames )
                    {
                        const auto& uniformVal = shaderProgram->getUniformValue( uniformName );
                        ImGui::Text( "ID: %d, Name: %s, Type: %s", uniformVal.Id, uniformVal.Name.cStr(), uniformVal.TypeName.cStr() );
                        if( uniformVal.Type == LOGLW::DataType::FLOAT )
                        {
                            ImGui::SameLine();
                            ImGui::Text( "val: %f", std::get<float>( uniformVal.Value ) );
                        }
                        if( uniformVal.Type == LOGLW::DataType::FLOAT_VEC3 )
                        {
                            glm::vec3 value = std::get<glm::vec3>( uniformVal.Value );

                            auto ManagerValue = [&fString, shaderProgram]( float& inOutValue, const CUL::String& uniformName )
                            {
                                bool changed{ false };

                                sprintf( stringBuffer, "%f", inOutValue );

                                ImGui::PushItemWidth( -1 );

                                if( ImGui::InputText( uniformName.cStr(), stringBuffer, stringBufferLength ) )
                                {
                                    fString = stringBuffer;
                                    if( fString.isFloat() )
                                    {
                                        inOutValue = fString.toFloat();
                                        changed = true;
                                        // shaderProgram->setUniform( uniformName, currentValue );
                                    }
                                }
                                else
                                {
                                    changed = false;
                                }

                                ImGui::PopItemWidth();
                                return changed;
                            };

                            bool hasValueChanged{ false };
                            hasValueChanged |= ManagerValue( value.x, uniformVal.Name + CUL::String( ".x" ) );
                            hasValueChanged |= ManagerValue( value.y, uniformVal.Name + CUL::String( ".y" ) );
                            hasValueChanged |= ManagerValue( value.z, uniformVal.Name + CUL::String( ".z" ) );

                            if( hasValueChanged )
                            {
                                shaderProgram->setUniform( EExecuteType::Now, uniformName, value );
                            }
                        }
                        else if( uniformVal.Type == LOGLW::DataType::FLOAT_VEC4 )
                        {
                            glm::vec4 value = std::get<glm::vec4>( uniformVal.Value );

                            auto ManagerValue = [&fString, shaderProgram]( float& inOutValue, const CUL::String& uniformName )
                            {
                                bool changed{ false };

                                sprintf( stringBuffer, "%f", inOutValue );

                                ImGui::PushItemWidth( -1 );

                                if( ImGui::InputText( uniformName.cStr(), stringBuffer, stringBufferLength ) )
                                {
                                    fString = stringBuffer;
                                    if( fString.isFloat() )
                                    {
                                        inOutValue = fString.toFloat();
                                        changed = true;
                                        // shaderProgram->setUniform( uniformName, currentValue );
                                    }
                                }
                                else
                                {
                                    changed = false;
                                }

                                ImGui::PopItemWidth();
                                return changed;
                            };

                            bool hasValueChanged{ false };
                            hasValueChanged |= ManagerValue( value.x, uniformVal.Name + CUL::String( ".x" ) );
                            hasValueChanged |= ManagerValue( value.y, uniformVal.Name + CUL::String( ".y" ) );
                            hasValueChanged |= ManagerValue( value.z, uniformVal.Name + CUL::String( ".z" ) );
                            hasValueChanged |= ManagerValue( value.w, uniformVal.Name + CUL::String( ".w" ) );

                            if( hasValueChanged )
                            {
                                shaderProgram->setUniform( EExecuteType::Now, uniformName, value );
                            }
                        }
                        else if( uniformVal.Type == LOGLW::DataType::FLOAT_MAT4 )
                        {
                            glm::mat4 currentValue = std::get<glm::mat4>( uniformVal.Value );
                            constexpr std::size_t bufferSize{ 32 };
                            char tableName[bufferSize];
                            snprintf( tableName, bufferSize, "## %s - %s", *shaderProgram->getName(), *uniformName );

                            static ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable |
                                                           ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                                                           ImGuiTableFlags_ContextMenuInBody;
                            if( ImGui::BeginTable( tableName, 4, flags ) )
                            {
                                for( std::int32_t row = 0; row < 4; ++row )
                                {
                                    ImGui::TableNextRow();
                                    for( std::int32_t column = 0; column < 4; ++column )
                                    {
                                        ImGui::TableSetColumnIndex( column );
                                        float& f0 = currentValue[row][column];
                                        fString = f0;
                                        constexpr std::size_t buffSize{ 32u };
                                        char buff[buffSize];
                                        strcpy( buff, fString.cStr() );
                                        ImGui::PushItemWidth( -1 );

                                        snprintf( labelBuffer, buffSize, "## r %d c %d", row, column );
                                        if( ImGui::InputText( labelBuffer, buff, buffSize ) )
                                        {
                                            fString = buff;
                                            if( fString.isFloat() )
                                            {
                                                f0 = fString.toFloat();
                                                shaderProgram->setUniform( EExecuteType::Now, uniformName, currentValue );
                                            }
                                        }
                                        ImGui::PopItemWidth();
                                    }
                                }
                                ImGui::EndTable();
                            }
                        }
                        else if( uniformVal.Type == LOGLW::DataType::SAMPLER_2D )
                        {
                            ImGui::SameLine();
                            ImGui::Text( "val: %d", std::get<std::int32_t>( uniformVal.Value ) );
                        }
                    }
                    ImGui::TreePop();
                }
            }
        }

        VertexArray* vao = currentObject->getVao();
        if( vao && ImGui::TreeNode( "Input Data" ) )
        {
            const std::size_t i{ 0u };
            LOGLW::VertexData& vertexData = vao->getVertexBuffer( i )->getData();
            const char* primitiveType = LOGLW::BasicTypes::toChar( vertexData.primitiveType );
            ImGui::Text( "Primitive type: %s", primitiveType );
            ImGui::Text( "VAO: %d, %s", vertexData.VAO, vao->getName().cStr() );
            ImGui::Text( "VBO: %d", vertexData.VBO );

            for( LOGLW::AttributeMeta& meta : vertexData.Attributes )
            {
                constexpr std::size_t bufferSize{ 256u };
                char attNameStr[bufferSize];
                snprintf( attNameStr, bufferSize, "Attribute: %d", meta.Index );

                if( ImGui::TreeNode( attNameStr ) )
                {
                    const std::int32_t dataTypSizeB = static_cast<std::int32_t>( LOGLW::BasicTypes::getSize( meta.Type ) );
                    const std::int32_t strideElCount = meta.StrideBytes / dataTypSizeB;

                    constexpr std::size_t textSize{ 16u };
                    char textInput[textSize];
                    snprintf( textInput, textSize, meta.Name.cStr() );
                    if( ImGui::InputText( "Name", textInput, textSize ) )
                    {
                        meta.Name = textInput;
                    }

                    ImGui::Text( "Index: %d", meta.Index );
                    ImGui::Text( "Size: %d", meta.Size );
                    const char* typeName = LOGLW::BasicTypes::toChar( meta.Type );
                    ImGui::Text( "Data type: %s", typeName );
                    ImGui::Text( "Normalized: %s", meta.Normalized ? "true" : "false" );
                    ImGui::Text( "Stride: %d (%d * %s)", meta.StrideBytes, strideElCount, LOGLW::BasicTypes::toChar( meta.Type ) );

                    std::int32_t columnCount = meta.Size;
                    std::int32_t rowsCount = static_cast<std::int32_t>( vertexData.Data.getElementCount() ) / strideElCount;

                    static ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable |
                                                   ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV |
                                                   ImGuiTableFlags_ContextMenuInBody;

                    if( ImGui::BeginTable( "Data", columnCount, flags ) )
                    {
                        std::uintptr_t offset = reinterpret_cast<std::uintptr_t>( meta.DataOffset );
                        std::uintptr_t dataVal = reinterpret_cast<std::uintptr_t>( vertexData.Data.getData() );

                        for( std::int32_t row = 0; row < rowsCount; ++row )
                        {
                            ImGui::TableNextRow();
                            for( std::int32_t column = 0; column < columnCount; ++column )
                            {
                                std::uintptr_t current = dataVal + row * meta.StrideBytes + offset + column * dataTypSizeB;
                                float* currentValueF = reinterpret_cast<float*>( current );
                                // currentValue = firstValue + + column + ( meta.Stride / dataTypSizeB );
                                float someFloat1{ 1.f };
                                float someFloat0{ 0.f };
                                float* someFloatAdd0 = &someFloat0;
                                (void*)someFloatAdd0;
                                float* someFloatAdd1 = &someFloat1;
                                (void*)someFloatAdd1;

                                constexpr std::size_t floatSize = sizeof( float );
                                (int)floatSize;

                                ImGui::TableSetColumnIndex( column );
                                // ImGui::Text( "%f", *currentValueF );
                                constexpr std::size_t bufferSize{ 32u };
                                char labelVal[bufferSize];
                                snprintf( labelVal, bufferSize, "val [ %d, %d ]", row, column );
                                if( ImGui::InputFloat( labelVal, currentValueF, 0.01f, 1.0f, "%4.4f" ) )
                                {
                                    // m_engine.getLoger()->log( "Clicked!" );
                                }
                            }
                        }
                        ImGui::EndTable();
                    }

                    (unsigned)columnCount;
                    (unsigned)rowsCount;

                    ImGui::TreePop();
                }
            }

            if( ImGui::Button( "Update vertex data." ) )
            {
                vao->updateVertexData( 0u );
            }

            ImGui::TreePop();
        }

        shownList.insert( currentObject );

        const std::set<IObject*>& children = currentObject->getChildren();
        if( children.empty() == false )
        {
            std::int32_t i{ 0 };
            for( IObject* child : children )
            {
                drawObjects( shownList, child, child->getName() );
            }
        }
        ImGui::TreePop();
    }
}

void GameEngineConcrete::drawSpriteData( Sprite* inSprite )
{
    if( ImGui::TreeNode( "UV" ) )
    {
        constexpr std::size_t labelBufferLength{ 32u };
        static char labelBuffer[labelBufferLength];

        auto uvList = inSprite->getUV();
        std::size_t i{ 0u };
        for( UV& uv : uvList )
        {
            sprintf( labelBuffer, "##_uv_%d", i );
            float values[2];
            values[0] = uv.X;
            values[1] = uv.Y;
            if( ImGui::InputFloat2( labelBuffer, values ) )
            {
                uv.X = values[0];
                uv.Y = values[1];

                inSprite->setUV( uv, i );
            }
            ++i;
        }

        ImGui::TreePop();
    }
}

void GameEngineConcrete::drawTransformData( LOGLW::TransformComponent* transform )
{
    glm::vec3 trans = transform->getPositionToParent();
    if( drawValues( trans, "trans" ) )
    {
        transform->setPositionToParent( trans );
    }

    glm::vec3 scale = transform->getScale();
    if( drawValues( scale, "scale" ) )
    {
        transform->setScale( scale );
    }

    ImGui::Text( "x: %4.2f y: %4.2f y: %4.2f", trans.x, trans.y, trans.z );

    const CUL::MATH::Rotation rot = transform->getRotationToParent();
    ImGui::Text( "Pitch: %4.2f Yaw: %4.2f Roll: %4.2f", rot.Pitch, rot.Yaw, rot.Roll );
}

bool GameEngineConcrete::drawValues( glm::vec3& inOutValue, const CUL::String& inName )
{
    ImGui::Text( "%s", inName.cStr() );
    constexpr float itemWidth{ 128.f };

    bool changed{ false };

    constexpr std::size_t stringBufferLength{ 32u };
    static char stringBuffer[stringBufferLength];

    constexpr std::size_t labelBufferLength{ 32u };
    static char labelBuffer[labelBufferLength];

    sprintf( labelBuffer, "##%s-xyz", inName.cStr() );

    ImGuiInputTextFlags flags{ 0 };
    changed = ImGui::InputFloat3( labelBuffer, &inOutValue.x, "%4.4f", flags );

    sprintf( labelBuffer, "##%s-s", inName.cStr() );

    constexpr float epsilon{ 0.00001 };
    if( CUL::MATH::Utils::equals( inOutValue.x, inOutValue.y, epsilon ) && CUL::MATH::Utils::equals( inOutValue.x, inOutValue.z, epsilon ) )
    {
        float valueAsOne = inOutValue.x;
        changed = ImGui::InputFloat( labelBuffer, &valueAsOne );
        if( changed )
        {
            inOutValue.x = inOutValue.y = inOutValue.z = valueAsOne;
        }
    }

    return changed;
}

void GameEngineConcrete::prepareProjection()
{
}

void GameEngineConcrete::changeProjectionType()
{
    m_renderDevice->resetMatrixToIdentity( MatrixTypes::PROJECTION );

    if( ProjectionType::ORTO == getCamera().getProjectionType() )
    {
        m_renderDevice->setOrthogonalPerspective( getCamera() );
    }
    else if( ProjectionType::PERSPECTIVE == getCamera().getProjectionType() )
    {
        m_renderDevice->setPerspectiveProjection( getCamera() );
    }
    m_renderDevice->resetMatrixToIdentity( MatrixTypes::MODELVIEW );
    m_renderDevice->lookAt( getCamera() );
    m_renderDevice->setDepthTest( getCamera().getDepthTestIsEnabled() );
}

void GameEngineConcrete::setEyePos( const glm::vec3& pos )
{
    getCamera().setEyePos( pos );
}

void GameEngineConcrete::renderObjects()
{
    ZoneScoped;
    std::lock_guard<std::mutex> lockGuard( m_objectsToRenderMtx );
    for( IRenderable* renderableObject : m_objectsToRender )
    {
        renderableObject->render();
    }
}

void GameEngineConcrete::finishFrame()
{
    ZoneScoped;
    m_renderDevice->finishFrame();

    if( m_updateBuffers )
    {
        m_activeWindow->updateScreenBuffers();
    }
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
            m_renderDevice->setViewport( m_viewport );
        }
    }
}

void GameEngineConcrete::setBackgroundColor( const ColorS& color )
{
    m_renderDevice->clearColorTo( color );
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

CUL::GUTILS::IConfigFile* GameEngineConcrete::getConfig()
{
    CUL::Assert::simple( m_sdlW != nullptr, "No proper SDL2 pointer initialized." );
    return m_sdlW->getConfig();
}

void GameEngineConcrete::drawDebugInfo( const bool enable )
{
    m_enableDebugDraw = enable;

    m_sdlW->getMainWindow()->toggleFpsCounter( enable );
}

IDebugOverlay* GameEngineConcrete::getDebugOverlay()
{
    return this;
}

void GameEngineConcrete::handleEvent( const SDL_Event& event )
{
    if( m_debugSystem )
    {
        ImGui_ImplSDL2_ProcessEvent( &event );
    }
}

void GameEngineConcrete::runEventLoop()
{
    m_sdlW->runEventLoop();
}

void GameEngineConcrete::stopEventLoop()
{
    m_sdlW->stopEventLoop();
}

LOGLW::IWindow* GameEngineConcrete::getMainWindow()
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

// LOGLW::IMouseObservable
void GameEngineConcrete::addMouseEventCallback( const LOGLW::IMouseObservable::MouseCallback& callback )
{
    m_sdlW->addMouseEventCallback( callback );
}

void GameEngineConcrete::registerMouseEventListener( LOGLW::IMouseObserver* observer )
{
    m_sdlW->registerMouseEventListener( observer );
}

void GameEngineConcrete::unregisterMouseEventListener( LOGLW::IMouseObserver* observer )
{
    m_sdlW->unregisterMouseEventListener( observer );
}

LOGLW::MouseData& GameEngineConcrete::getMouseData()
{
    return m_sdlW->getMouseData();
}

// LOGLW::IKeyboardObservable
void GameEngineConcrete::registerKeyboardEventCallback( const std::function<void( const LOGLW::KeyboardState& keyboardState )>& callback )
{
    m_sdlW->registerKeyboardEventCallback( callback );
}

void GameEngineConcrete::registerKeyboardEventListener( LOGLW::IKeyboardObserver* observer )
{
    m_sdlW->registerKeyboardEventListener( observer );
}

void GameEngineConcrete::unregisterKeyboardEventListener( LOGLW::IKeyboardObserver* observer )
{
    m_sdlW->unregisterKeyboardEventListener( observer );
}

bool GameEngineConcrete::isKeyUp( const String& keyName ) const
{
    return m_sdlW->isKeyUp( keyName );
}

void GameEngineConcrete::registerWindowEventCallback( const LOGLW::WindowCallback& callback )
{
    m_sdlW->registerWindowEventCallback( callback );
}

CUL::CULInterface* GameEngineConcrete::getCul()
{
    return m_sdlW->getCul();
}

void GameEngineConcrete::setFpsLimit( float maxFps )
{
    FrameTimeManager::getInstance().setTargetFPS( maxFps );
}

float GameEngineConcrete::getFpsLimit() const
{
    return FrameTimeManager::getInstance().getTargetFPS();
}

void GameEngineConcrete::runPreRenderTasks()
{
    ZoneScoped;
    std::lock_guard<std::mutex> lock( m_preRenderTasksMtx );
    while( !m_preRenderTasks.empty() )
    {
        auto task = m_preRenderTasks.top();
        m_preRenderTasks.pop();
        task();
    }
}

void GameEngineConcrete::runPostRenderTasks()
{
    ZoneScoped;
    std::lock_guard<std::mutex> lock( m_postRenderTasksMtx );
    while( !m_postRenderTasks.empty() )
    {
        auto task = m_postRenderTasks.top();
        m_postRenderTasks.pop();
        task();
    }
}

GameEngineConcrete::~GameEngineConcrete()
{
    m_logger->log( "GameEngineConcrete::~GameEngineConcrete()..." );

    m_logger->log( "GameEngineConcrete::~GameEngineConcrete() Done." );
}