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
#include "sdl2wrapper/Input/MouseData.hpp"

#include "CUL/ITimer.hpp"

CUL::MATH::Angle ang90( 90, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang180( 180, CUL::MATH::Angle::Type::DEGREE );
CUL::MATH::Angle ang270( 270, CUL::MATH::Angle::Type::DEGREE );

void Playground::run()
{
    auto width = 1280;
    auto height = 600;

    CUL::Graphics::Pos2Di winPos = { 200, 200 };
    LOGLW::WinSize winSize = { width, height };

    LOGLW::EngineParams engineParams;
    engineParams.configPath = "Config.txt";
    engineParams.legacy = false;
    engineParams.windowPosition = winPos;
    engineParams.winSize = winSize;
    engineParams.winName = "gameenginePlaygroundApp";

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
    m_engine->drawOrigin( true );
    m_engine->drawDebugInfo( true );
    m_engine->startRenderingLoop();

    m_timer.reset( CUL::TimerFactory::getChronoTimer( m_engine->getLoger() ) );

    m_engine->runEventLoop();
}

void Playground::afterInit()
{
    // m_mainWindow = m_engine->getMainWindow();
    // m_mainWindow->setBackgroundColor( LOGLW::ColorS( 1.0f, 0.0f, 0.0f, 1.0f ) );
    // const auto& winSize = m_mainWindow->getSize();

    // m_camera = &m_engine->getCamera();

    // m_camera->setSize( { winSize.getWidth(), winSize.getHeight() } );
    // m_camera->setEyePos( { 0.0f, 0.0f, 32.f } );
    // m_camera->setCenter( { 0.f, 0.f, -10.f } );
    // m_camera->setZNear( 1.f );

    // m_mainWindow->toggleFpsCounter( true, 8u );

    // m_engine->drawDebugInfo( true );
    // m_engine->drawOrigin( true );

    // m_engine->getDebugOverlay()->addSliderValue( "Blue Z", &blueTriangleZ, -64.0f, 128.f, [] (){
    // } );

    // m_engine->getDebugOverlay()->addSliderValue( "Red Z", &redTriangleZ, -64.0f, 128.f, [] (){
    // } );

    // const float size = 32.f;
    // LOGLW::TriangleData values;
    // values[0] = LOGLW::PointType( size, -size, 0.0f );
    // values[1] = LOGLW::PointType{ -size, -size, 0.0f };
    // values[2] = LOGLW::PointType{ -size, size, 0.0f };

    // const float z = 26.f;
    // const float x = 1.f;
    // const float yTriangle = -1.f;
    // const float yCube = -3.f;

    // m_triangleModern = m_engine->createTriangle( nullptr, false );
    // m_triangleModern->setColor( LOGLW::ColorE::RED );
    // m_triangleModern->getTransform()->setPositionAbsolute( { -x, yTriangle, z } );
    // m_triangleModern->setName( "m_triangleModern" );

    // m_triangleLegacy = m_engine->createTriangle( nullptr, true );
    // m_triangleLegacy->setColor( LOGLW::ColorE::BLUE );
    // m_triangleLegacy->getTransform()->setPositionAbsolute( { x, yTriangle, z } );
    // m_triangleLegacy->setName( "m_triangleLegacy" );
    //

    // m_quadModern = m_engine->createQuad( nullptr );
    // m_quadModern->setName( "m_quadModern" );
    // m_quadModern->setColor( CUL::Graphics::ColorE::RED );
    // m_quadModern->getTransform()->setPositionAbsolute( { 0.f, 4.f, z } );
    // m_quadModern->getTransform()->setPivot( { 1.f, 0.5f, 0.f } );
    // m_quadModern->getTransform()->setScale( { 1.f, 1.f, 0.f } );

    // m_quadLegacy = m_engine->createQuad( nullptr, true );
    // m_quadLegacy->setName( "m_quadLegacy" );
    // m_quadLegacy->setColor( CUL::Graphics::ColorE::BLUE );
    // m_quadLegacy->getTransform()->setPositionAbsolute( { 0.f, 2.f, z } );
    // m_quadLegacy->getTransform()->setPivot( { 1.f, 0.5f, 0.f } );
    // m_quadLegacy->getTransform()->setScale( { 1.f, 1.f, 0.f } );

    // g_sprite = m_engine->getObjectFactory()->createSprite( "../../media/texture.png" );
    // g_sprite->setName( "g_sprite" );
    // g_sprite->getTransform()->setPositionToParent( { -0.2f, 0.f, 4.f } );

    // m_cubeModern = m_engine->createCube( false );
    // m_cubeModern->getTransform()->setPositionAbsolute( { -x, yCube, z } );
    // m_cubeModern->setColor( CUL::Graphics::ColorE::BLUE );

    // m_cubeLegacy = m_engine->createCube( true );
    // m_cubeLegacy->getTransform()->setPositionAbsolute( { x, yCube, z } );
    // m_cubeLegacy->setColor( CUL::Graphics::ColorE::RED );

    // m_timer->runEveryPeriod( [this] (){timer(); }, 40000 );

    HRESULT hr = D3D12CreateDevice( nullptr, D3D_FEATURE_LEVEL_11_0, __uuidof( ID3D12Device ), (void**)&m_device );

    D3D12_FEATURE_DATA_D3D12_OPTIONS options;
    hr = m_device->CheckFeatureSupport( D3D12_FEATURE_D3D12_OPTIONS, reinterpret_cast<void*>( &options ), sizeof( options ) );
    if( FAILED( hr ) )
    {
        throw;
    }
#pragma message( "TODO" )
    // https://github.com/galek/SDL-Directx12/blob/master/main.cpp

    // Create the command allocator and queue objects.Then, obtain command lists from the command allocator and submit them to the command
    // queue. This example shows calling ID3D12Device::CreateCommandAllocator and ID3D12Device::GetDefaultCommandQueue. Latter doesn't exist
    // tho??
    hr = m_device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof( ID3D12CommandAllocator ),
                                           (void**)&m_commandListAllocator );
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    hr = m_device->CreateCommandQueue( &commandQueueDesc, __uuidof( ID3D12CommandQueue ), (void**)&mCommandQueue );
    // HWND hwnd;
    //// Create the swap chain descriptor.
    // DXGI_SWAP_CHAIN_DESC swapChainDesc;
    // ZeroMemory( &swapChainDesc, sizeof( swapChainDesc ) );
    // swapChainDesc.BufferCount = g_bbCount;
    // swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    // swapChainDesc.OutputWindow = hWnd;
    // swapChainDesc.SampleDesc.Count = 1;
    // swapChainDesc.Windowed = TRUE;
    // swapChainDesc.Flags = 0; //DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
    ////swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    // swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
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
        const auto winW = winSize.getWidth();
        const auto winH = winSize.getHeight();
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
    m_engine->stopRenderingLoop();
    m_engine->stopEventLoop();
}