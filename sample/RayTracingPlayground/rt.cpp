#include "rt.hpp"

#include "gameengine/IGameEngine.hpp"
#include "gameengine/EngineParams.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/IDebugOverlay.hpp"
#include "gameengine/Primitives/Triangle.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Cube.hpp"
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
    m_sphere.set_properties( Vec3f( 0.f, 0.f, -32.f ), 3.f, 128u );

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
    m_widget = dynamic_cast<LOGLW::IWidgetEditable*>( m_engine->getUIService().createWidget() );

    g_triangle = m_engine->createTriangle( nullptr );
    m_timer->runEveryPeriod(
        [this]()
        {
            timer();
        },
        40000 );
    auto fb = m_engine->getFrameBuffer();
    fb->setSize( m_mainWindow->getSize().W, m_mainWindow->getSize().H );
}

void RT_Playground::timer()
{
    const auto size = m_engine->getMainWindow()->getSize();
    constexpr float screenZ = 0.f;  // screen plane at z = 0
    const std::size_t width = size.W;
    const std::size_t height = size.H;

    static LOGLW::S_RGBA_F pix{ 0.f, 0.f, 0.f, 1.f };
    static glm::vec3 eye{ 0.f, 0.f, 4.f };

    for( size_t j = 0; j < height; j++ )
    {
        for( size_t i = 0; i < width; i++ )
        {
            std::size_t type{ 1u };
            if( type == 0u )
            {
                m_widget->updatePixel( i, j, pix );
            }
            else if( type == 1u )
            {
                float ndcX = ( i + 0.5f ) / width * 2.f - 1.f;
                float ndcY = 1.f - ( j + 0.5f ) / height * 2.f;

                glm::vec3 pointOnScreen = glm::vec3( ndcX, ndcY, screenZ );
                glm::vec3 rayDir = glm::normalize( pointOnScreen - eye );

                glm::vec3 intersectionPoint;
                glm::vec3 intersectionNormal;
                if( glm::intersectRaySphere( eye,                   // ray origin
                                             rayDir,                // normalized direction
                                             m_sphere.getCenter(),  // sphere center
                                             m_sphere.getRadius(),  // radius
                                             intersectionPoint,     // output hit position
                                             intersectionNormal     // output normal
                                             ) )
                {
                    pix.Red = 1.f;
                    pix.Green = 0.f;
                    pix.Blue = 0.f;
                }
                else
                {
                    pix.Red = 0.f;
                    pix.Green = 0.f;
                    pix.Blue = 0.f;
                }

                m_widget->updatePixel( i, j, pix );
            }
            else
            {
                Vec3f dir = Vec3f( i, j, -1 );
                dir = glm::normalize( dir );

                pix.Red = j / float( height );
                pix.Green = i / float( width );
                pix.Blue = 0.f;
                m_widget->updatePixel( i, j, pix );
            }
        }
    }

    m_time += 0.01f;
}

void RT_Playground::onMouseEvent( const LOGLW::MouseData& mouseData )
{
    if( mouseData.isButtonDown( 1 ) )
    {
        m_engine->getLoger()->logInfo( "Mouse: %d, %d", mouseData.getX(), mouseData.getY() );
        static LOGLW::S_RGBA_F color{ 1.f, 0.f, 0.f, 1.f };
        m_widget->updatePixel( static_cast<std::size_t>( mouseData.getX() ), static_cast<std::size_t>( mouseData.getY() ), color );
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
        m_sphere.move( Vec3f( 0.f, 0.f, -1.0f ) );
        m_engine->getLoger()->logInfo( "Sphere pos: %f, %f, %f", m_sphere.getCenter().x, m_sphere.getCenter().y, m_sphere.getCenter().z );
    }
    else if( key.at( "S" ) )
    {
        m_sphere.move( Vec3f( 0.f, 0.f, 1.0f ) );
        m_engine->getLoger()->logInfo( "Sphere pos: %f, %f, %f", m_sphere.getCenter().x, m_sphere.getCenter().y, m_sphere.getCenter().z );
    }
    else if( key.at( "U" ) )
    {
        m_sphere.resize( 2.f );
        m_engine->getLoger()->logInfo( "Sphere radius: %f", m_sphere.getRadius() );
    }
    else if( key.at( "J" ) )
    {
        m_sphere.resize( 0.5f );
        m_engine->getLoger()->logInfo( "Sphere radius: %f", m_sphere.getRadius() );
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

void Sphere::set_properties( const Vec3f& c, const float& r, std::size_t pointsCount )
{
    m_center = c;
    m_radius = r;
    m_pointsCount = pointsCount;
}

std::size_t Sphere::getPointsCount() const
{
    return m_pointsCount;
}

glm::vec3 Sphere::getPoint( std::size_t i ) const
{
    const float goldenAngle = glm::pi<float>() * ( 3.0f - std::sqrt( 5.0f ) );

    float t = static_cast<float>( i ) / static_cast<float>( m_pointsCount );

    float y = 1.0f - 2.0f * t;
    float r = std::sqrt( 1.0f - y * y );

    float theta = goldenAngle * static_cast<float>( i );

    float x = r * std::cos( theta );
    float z = r * std::sin( theta );

    return m_center + m_radius * glm::vec3( x, y, z );
}

bool Sphere::ray_intersect( const Vec3f& orig, const Vec3f& dir, float& t0 ) const
{
    Vec3f L = m_center - orig;
    float tca = glm::length( L * dir );
    float d2 = glm::length( L * L ) - tca * tca;
    if( d2 > m_radius * m_radius )
        return false;
    float thc = sqrtf( m_radius * m_radius - d2 );
    t0 = tca - thc;
    float t1 = tca + thc;
    if( t0 < 0 )
        t0 = t1;
    if( t0 < 0 )
        return false;
    return true;
}

Vec3f cast_ray( const Vec3f& orig, const Vec3f& dir, const Sphere& sphere )
{
    float sphere_dist = std::numeric_limits<float>::max();
    if( !sphere.ray_intersect( orig, dir, sphere_dist ) )
    {
        return Vec3f( 0.2, 0.7, 0.8 );  // background color
    }
    return Vec3f( 0.4, 0.4, 0.3 );
}