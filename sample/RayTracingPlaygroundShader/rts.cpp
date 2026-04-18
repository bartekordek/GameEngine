#include "rts.hpp"

#include "gameengine/Camera.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Cube.hpp"
#include "gameengine/EngineParams.hpp"
#include "gameengine/ExecuteType.hpp"
#include "gameengine/IDebugOverlay.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/Input/MouseData.hpp"
#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Primitives/Sphere.hpp"
#include "gameengine/Primitives/Triangle.hpp"
#include "gameengine/Render/ITextureFrameBuffer.hpp"
#include "gameengine/Render/PixelFormats.hpp"
#include "gameengine/Render/PointLight.hpp"
#include "gameengine/Scene/Scene.hpp"
#include "gameengine/Scene/SceneStore.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/Sprite.hpp"
#include "gameengine/UI/UIService.hpp"
#include "gameengine/UI/WidgetEditable.hpp"
#include "gameengine/Windowing/IWindow.hpp"

#include "CUL/ITimer.hpp"

CUL::MATH::Angle ang90( 90, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang180( 180, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang270( 270, CUL::MATH::Angle::Type::DEGREE );


RT_Playground::RT_Playground( const LOGLW::WinData& inWinData ) : m_winData( inWinData )
{
}

void RT_Playground::run()
{
    LOGLW::EngineParams engineParams;
    engineParams.ConfigPath = "Config.txt";
    engineParams.legacy = false;
    engineParams.WinDataVal = m_winData;
    engineParams.WinDataVal.Name = "gameenginePlaygroundApp";

    m_engine = LOGLW::IGameEngine::createGameEngine( engineParams );

    m_engine->addMouseEventCallback(
        [this]( const LOGLW::MouseData& mouseData )
        {
            onMouseEvent( mouseData );
        } );

    m_engine->onInitialize(
        [this]()
        {
            afterInit();
        } );

    m_engine->registerKeyboardEventCallback(
        [this]( const LOGLW::KeyboardState& key )
        {
            onKeyBoardEvent( key );
        } );
    m_engine->registerWindowEventCallback(
        [this]( const LOGLW::WindowEvent::Type type )
        {
            onWindowEvent( type );
        } );
    m_engine->startRenderingLoop();

    m_timer.reset( CUL::TimerFactory::getChronoTimerPtr( m_engine->getLoger() ) );

    m_engine->runEventLoop();
}

void RT_Playground::afterInit()
{
    m_scene = m_engine->getSceneStore().createScene( "main" );


    m_mainWindow = m_engine->getMainWindow();
    m_mainWindow->setBackgroundColor( LOGLW::ColorS( 1.0f, 0.0f, 0.0f, 1.0f ) );
    const auto& winSize = m_mainWindow->getSize();

    m_camera = &m_engine->getCamera();

    m_camera->setSize( { winSize.W, winSize.H } );
    m_camera->setEyePos( { 0.0f, 0.0f, 32.f } );
    m_camera->setCenter( { 0.f, 0.f, -10.f } );
    m_camera->setZNear( 1.f );

    m_mainWindow->toggleFpsCounter( true, 8u );

    m_engine->drawDebugInfo( false );
    m_engine->drawOrigin( false );

    m_timer->runEveryPeriod(
        [this]()
        {
            timer();
        },
        40000 );
    auto fb = m_engine->getFrameBuffer();
    fb->setSize( m_mainWindow->getSize().W, m_mainWindow->getSize().H );

    {
        m_bulb = m_scene->createPointLight( nullptr );
        m_bulb->setColor( CUL::Graphics::ColorE::YELLOW );
        m_bulb->getTransform()->setPivot( { 0.5f, 0.5f, 0.f } );
        m_bulb->getTransform()->setPositionToParent( { 0.f, 0.f, 2.f } );
        m_bulb->getTransform()->setSize( { 0.9f, 0.9f, 0.9f } );
        constexpr float initialScale = 0.02f;
        m_bulb->getTransform()->setScale( { initialScale, initialScale, initialScale } );
        m_bulb->setName( "m_bulb" );
    }

    {
        m_quad = m_scene->createQuad( nullptr );
        m_quad->setColor( CUL::Graphics::ColorE::YELLOW );
        m_quad->setName( "Wall_with_light" );
        LOGLW::ShaderProgram::ShadersData sd;
        sd.FragmentShader = "embedded_shaders/rt_basic.frag";
        sd.VertexShader = "embedded_shaders/rt_basic.vert";
        m_quad->getProgram()->createFrom( LOGLW::EExecuteType::Now, sd );
        m_quad->getProgram()->setUniform(
            LOGLW::EExecuteType::Now, "eyePos", m_engine->getCamera().getEye() );
        m_quad->getProgram()->setUniform(
            LOGLW::EExecuteType::Now, "lightPos", m_bulb->getTransform()->getPositionAbsolut() );
        m_quad->getTransform()->setSize( { 40.f, 40.f, 1.f } );
    }

    {
        m_sphere = m_scene->createSphere( nullptr );
        m_sphere->getTransform()->setPositionAbsolute({ 8.f, 8.f, 8.f });
    }
}

void RT_Playground::timer()
{
    m_time += 0.01f;
}

void RT_Playground::onMouseEvent( const LOGLW::MouseData& mouseData )
{
    if( mouseData.isButtonDown( 1 ) )
    {
        m_engine->getLoger()->logInfo(
            "Mouse: %d, %d", mouseData.getX(), mouseData.getY() );
    }
    else if( mouseData.isButtonDown( 3 ) )
    {
        const auto& md = m_engine->getMouseData();
        const auto& winSize = m_engine->getMainWindow()->getSize();
        const auto winW = winSize.W;
        const auto winH = winSize.H;
        const auto mouseX = md.getX();
        const auto mouseY = md.getY();
        const auto centerX = winW / 2 - mouseX;
        const auto centerY = winH / 2 - mouseY;

        const auto rectW = winW / 2;
        const auto rectH = winH / 2;

        const auto leftX = ( winW - rectW ) / 2;
        const auto rightX = winW - leftX;

        const auto bottom = ( winH - rectH ) / 2;
        const auto top = winH - bottom;

        if( mouseX < rightX && mouseX > leftX && mouseY < top && mouseY > bottom )

        {
            auto eye = m_engine->getCamera().getEye();
            static auto delta = 0.5f;
            eye.x = +centerX * delta;
            eye.y = -centerY * delta;
            m_engine->setEyePos( eye );
        }
    }
}

void RT_Playground::onKeyBoardEvent( const LOGLW::KeyboardState& key )
{
    constexpr float moveDelta = 2.0f;

    if( key.at( "Q" ) )
    {
        closeApp();
    }

    if( key.at( "W" ) )
    {
        m_bulb->getTransform()->move( { 0.f, moveDelta, 0.f } );
    }
    else if( key.at( "S" ) )
    {
        m_bulb->getTransform()->move( { 0.f, -moveDelta, 0.f } );
    }
    else if( key.at( "A" ) )
    {
        m_bulb->getTransform()->move( { -moveDelta, 0.f, 0.f } );
    }
    else if( key.at( "D" ) )
    {
        m_bulb->getTransform()->move( { moveDelta, 0.f, 0.f } );
    }
    else if( key.at( "=" ) )
    {
        m_bulb->getTransform()->move( { 0.f, 0.f, moveDelta } );
    }
    else if( key.at( "-" ) )
    {
        m_bulb->getTransform()->move( { 0.f, 0.f, -moveDelta } );
    }
    else if( key.at( "U" ) )
    {
    }
    else if( key.at( "J" ) )
    {
    }
}

void RT_Playground::onWindowEvent( const LOGLW::WindowEvent::Type type )
{
    if( LOGLW::WindowEvent::Type::CLOSE == type )
    {
        closeApp();
    }
}

void RT_Playground::closeApp()
{
    m_timer->stop();

    m_engine->stopRenderingLoop();
    m_engine->stopEventLoop();
}