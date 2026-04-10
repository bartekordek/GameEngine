#include "rts.hpp"

#include "gameengine/IGameEngine.hpp"
#include "gameengine/EngineParams.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/IDebugOverlay.hpp"
#include "gameengine/Primitives/Triangle.hpp"
#include "gameengine/ExecuteType.hpp"
#include "gameengine/Render/PointLight.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Cube.hpp"
#include "gameengine/Primitives/Sphere.hpp"
#include "gameengine/Render/ITextureFrameBuffer.hpp"
#include "gameengine/Render/PixelFormats.hpp"
#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Render/PixelFormats.hpp"
#include "gameengine/Sprite.hpp"
#include "gameengine/UI/UIService.hpp"
#include "gameengine/UI/WidgetEditable.hpp"

#include "gameengine/Windowing/IWindow.hpp"
#include "gameengine/Input/MouseData.hpp"

#include "CUL/ITimer.hpp"

CUL::MATH::Angle ang90( 90, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang180( 180, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang270( 270, CUL::MATH::Angle::Type::DEGREE );

LOGLW::Triangle* g_triangle{ nullptr };

RT_Playground::RT_Playground( const LOGLW::WinData& inWinData ):
    m_winData( inWinData )
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

    g_triangle = m_engine->createTriangle( nullptr );
    m_timer->runEveryPeriod(
        [this]()
        {
            timer();
        },
        40000 );
    auto fb = m_engine->getFrameBuffer();
    fb->setSize( m_mainWindow->getSize().W, m_mainWindow->getSize().H );

    m_sphere = new LOGLW::CSphere( nullptr );

    LOGLW::ShaderProgram::ShadersData sd;
    sd.FragmentShader = "embedded_shaders/basic_color.frag";
    sd.VertexShader = "embedded_shaders/basic_pos.vert";

    m_sphere->getProgram()->createFrom( LOGLW::EExecuteType::WaitForCompletion, sd );

    m_pointLight = m_engine->createPointLight( nullptr );
}

void RT_Playground::timer()
{


    m_time += 0.01f;
}

void RT_Playground::onMouseEvent( const LOGLW::MouseData& mouseData )
{
    if( mouseData.isButtonDown( 1 ) )
    {
        m_engine->getLoger()->logInfo( "Mouse: %d, %d", mouseData.getX(), mouseData.getY() );
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
    if( key.at( "Q" ) )
    {
        closeApp();
    }

    if( key.at( "W" ) )
    {
    }
    else if( key.at( "S" ) )
    {
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