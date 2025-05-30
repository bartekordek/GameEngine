#include "Playground.hpp"

#include "gameengine/IGameEngine.hpp"
#include "gameengine/EngineParams.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/IDebugOverlay.hpp"
#include "gameengine/Primitives/Triangle.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Cube.hpp"
#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Sprite.hpp"

#include "gameengine/Windowing/IWindow.hpp"
#include "gameengine/Input/MouseData.hpp"

#include "CUL/ITimer.hpp"

CUL::MATH::Angle ang90( 90, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang180( 180, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang270( 270, CUL::MATH::Angle::Type::DEGREE );

Playground::Playground( const LOGLW::WinData& inWinData ) : m_winData( inWinData )
{
}

void Playground::run()
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

void Playground::afterInit()
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

    m_engine->drawDebugInfo( true );
    m_engine->drawOrigin( true );

    const float z = 26.f;
    const float x = 1.f;
    const float yTriangle = -1.f;
    const float yCube = -3.f;

    m_triangleModern = m_engine->createTriangle( nullptr, false );
    m_triangleModern->setColor( LOGLW::ColorE::RED );
    m_triangleModern->getTransform()->setPositionAbsolute( { -x, yTriangle, z } );
    m_triangleModern->setName( "m_triangleModern" );

    m_triangleLegacy = m_engine->createTriangle( nullptr, false );
    m_triangleLegacy->setColor( LOGLW::ColorE::BLUE );
    m_triangleLegacy->getTransform()->setPositionAbsolute( { x, yTriangle, z } );
    m_triangleLegacy->setName( "m_triangleLegacy" );

    m_quadModern = m_engine->createQuad( nullptr );
    m_quadModern->setName( "m_quadModern" );
    m_quadModern->setColor( CUL::Graphics::ColorE::RED );
    m_quadModern->getTransform()->setPositionAbsolute( { 0.f, 4.f, z } );
    m_quadModern->getTransform()->setPivot( { 1.f, 0.5f, 0.f } );
    m_quadModern->getTransform()->setScale( { 1.f, 1.f, 0.f } );

    m_quadLegacy = m_engine->createQuad( nullptr, false );
    m_quadLegacy->setName( "m_quadLegacy" );
    m_quadLegacy->setColor( CUL::Graphics::ColorE::BLUE );
    m_quadLegacy->getTransform()->setPositionAbsolute( { 0.f, 2.f, z } );
    m_quadLegacy->getTransform()->setPivot( { 1.f, 0.5f, 0.f } );
    m_quadLegacy->getTransform()->setScale( { 1.f, 1.f, 0.f } );

    g_sprite = m_engine->createSprite( "../../media/texture.png" );
    g_sprite->setName( "g_sprite" );
    g_sprite->getTransform()->setPositionToParent( { -0.2f, 0.f, 24.f } );

    m_cubeModern = m_engine->createCube( false );
    m_cubeModern->getTransform()->setPositionAbsolute( { -x, yCube, z } );
    m_cubeModern->setColor( CUL::Graphics::ColorE::BLUE );
    m_cubeModern->setName( "m_cubeModern" );

    m_cubeLegacy = m_engine->createCube( false );
    m_cubeLegacy->getTransform()->setPositionAbsolute( { x, yCube, z } );
    m_cubeLegacy->setColor( CUL::Graphics::ColorE::RED );
    m_cubeLegacy->setName( "m_cubeLegacy" );

    m_timer->runEveryPeriod(
        [this]()
        {
            timer();
        },
        40000 );
}

void Playground::timer()
{
    static CUL::MATH::Rotation rotation;
    rotation.reset();

    rotation.Yaw = m_angle;
    const float t = ( 1.f + std::sin( m_time ) ) / 2.f;
    const float ampplitude = t * 0.9f + 0.1f;

    if( m_quadLegacy )
    {
        // m_quadLegacy->getTransform()->setRotationToParent( rotation );
        m_quadLegacy->getTransform()->setScale( { ampplitude, ampplitude, 0.f } );
    }

    if( m_quadModern )
    {
        // m_quadModern->getTransform()->setRotationToParent( rotation );
        m_quadModern->getTransform()->setScale( { ampplitude, ampplitude, 0.f } );
    }

    rotation.reset();
    rotation.Pitch = m_angle;
    m_triangleModern->getTransform()->setRotationToParent( rotation );
    m_triangleLegacy->getTransform()->setRotationToParent( rotation );

    rotation.reset();
    rotation.Roll = m_angle;
    if( m_cubeModern )
    {
        m_cubeModern->getTransform()->setRotationToParent( rotation );
    }

    if( m_cubeLegacy )
    {
        m_cubeLegacy->getTransform()->setRotationToParent( rotation );
    }

    m_angle += 0.01f;
    m_time += 0.01f;
}

void Playground::onMouseEvent( const LOGLW::MouseData& mouseData )
{
    if( mouseData.isButtonDown( 3 ) )
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

void Playground::onKeyBoardEvent( const LOGLW::KeyboardState& key )
{
    const auto deltaZ = 1.0f;
    const auto delta = 2.0f;

    if( key.at( "Q" ) )
    {
        closeApp();
    }

    if( key.at( "W" ) )
    {
        blueTriangleZ -= deltaZ;
    }

    if( key.at( "S" ) )
    {
        blueTriangleZ += deltaZ;
    }

    if( key.at( "U" ) )
    {
        auto newVal = m_camera->getZfar() + delta;

        glm::vec3 center = m_camera->getCenter();
        center.z = newVal;
        m_camera->setCenter( center );
    }

    if( key.at( "J" ) )
    {
        auto newVal = m_camera->getZfar() - delta;

        glm::vec3 center = m_camera->getCenter();
        center.z = newVal;
        m_camera->setCenter( center );
    }

    if( key.at( "I" ) )
    {
        g_eyePos = m_engine->getCamera().getEye();
        g_eyePos.z += 2.0f;
        m_engine->getCamera().setEyePos( g_eyePos );
    }

    if( key.at( "K" ) )
    {
        g_eyePos = m_engine->getCamera().getEye();
        g_eyePos.z -= 2.0f;
        m_engine->getCamera().setEyePos( g_eyePos );
    }

    if( key.at( "T" ) )
    {
        redTriangleZ -= deltaZ;
    }

    if( key.at( "G" ) )
    {
        redTriangleZ += deltaZ;
    }

    if( key.at( "P" ) )
    {
        static bool toggle = true;
        toggle = !toggle;
    }

    if( key.at( "," ) )
    {
        glm::vec3 scale = g_sprite->getTransform()->getScale();
        scale *= 0.2f;
        g_sprite->getTransform()->setScale( scale );
    }

    if( key.at( "." ) )
    {
        glm::vec3 scale = g_sprite->getTransform()->getScale();
        scale /= 0.2f;
        g_sprite->getTransform()->setScale( scale );
    }
}

void Playground::onWindowEvent( const LOGLW::WindowEvent::Type type )
{
    if( LOGLW::WindowEvent::Type::CLOSE == type )
    {
        closeApp();
    }
}

void Playground::closeApp()
{
    m_timer->stop();
    delete m_quadModern;
    m_quadModern = nullptr;
    delete m_quadLegacy;
    m_quadLegacy = nullptr;

    m_engine->stopRenderingLoop();
    m_engine->stopEventLoop();
}