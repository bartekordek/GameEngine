#include "gameengine/IGameEngineApp.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/Windowing/IWindow.hpp"

#include "CUL/Time.hpp"

using namespace LOGLW;

IGameEngineApp::IGameEngineApp( bool fullscreen, unsigned width, unsigned height, int x, int y, const char* winName, const char* configPath,
                                bool legacy )
{
    LOGLW::WinData windowData;
    windowData.Name = "Test";
    windowData.Pos = { x, y };
    windowData.CurrentRes = { (std::uint16_t)width, (std::uint16_t)height };
    windowData.Name = winName;

    init( windowData, fullscreen, configPath, legacy );
}

IGameEngineApp::IGameEngineApp( bool fullscreen, unsigned width, unsigned height, WinOrientation orientation, const char* winName,
                                const char* configPath,
                                bool legacy )
{
    LOGLW::WinData windowData;
    windowData.Name = "Test";
    windowData.CurrentRes = { (std::uint16_t)width, (std::uint16_t)height };
    windowData.Name = winName;

    init( windowData, fullscreen, configPath, legacy );

    if( orientation == LOGLW::IGameEngineApp::WinOrientation::CENTER )
    {
        auto screenSize = m_sdlw->getMainWindow()->getCurrentScreenNativeResolution();
        WinPos newPos{ static_cast<std::int32_t>( ( screenSize.W - width ) / 2 ),
                       static_cast<std::int32_t>( ( screenSize.H - height ) / 2 ) };
        m_sdlw->getMainWindow()->setPos( newPos );
    }
}

void IGameEngineApp::init( const LOGLW::WinData& windowData, bool fullscreen, const char* configPath, bool legacy )
{
    m_sdlw.reset( LOGLW::ISDL2Wrapper::createSDL2Wrapper() );
    m_sdlw->init( windowData, configPath );
    m_sdlw->registerWindowEventListener( this );
    m_sdlw->registerKeyboardEventListener( this );
    m_sdlw->registerMouseEventListener( this );

    m_oglw.reset( LOGLW::IGameEngine::createGameEngine( m_sdlw.get(), legacy ) );
    m_logger = m_oglw->getLoger();
    m_device = m_oglw->getDevice();

    m_oglw->onInitialize(
        [this]()
        {
            onInit();
        } );
    m_oglw->beforeFrame(
        [this]()
        {
            customFrame();
        } );

    m_sdlw->getMainWindow()->setFullscreen( fullscreen );

    // LOGLW::ProjectionData g_projectionData;
    // const auto& winSize = m_sdlw->getMainWindow()->getSize();
    // g_projectionData.setSize( { winSize.getWidth(), winSize.getHeight() } );
    // g_projectionData.setZNear( 127.0f );
    // g_projectionData.setZfar( -64.0f );
    // g_projectionData.setCenter( { 0.f, 0.f } );
    // g_projectionData.setEyePos( { 0.f, 0.f, 128.f } );
    // g_projectionData.m_projectionType = LOGLW::ProjectionType::PERSPECTIVE;

    m_logicTimer.reset( CUL::TimerFactory::getChronoTimerPtr( m_logger ) );
    m_logicThread = std::thread( &IGameEngineApp::logicThread, this );
}

void IGameEngineApp::run()
{
    m_oglw->startRenderingLoop();
    m_sdlw->runEventLoop();
}

void IGameEngineApp::logicThread()
{
    m_logicTimer->start();
    unsigned int msDuration = 0;
    while( m_runLogicThread )
    {
        m_logicTimer->stop();
        msDuration = m_logicTimer->getElapsed().getMs();
        customLogicThreadFrame( msDuration );
        m_logicTimer->start();
        CUL::ITimer::sleepMiliSeconds( m_logicThreadSleepMs );
    }
}

void IGameEngineApp::onWindowEvent( const LOGLW::WindowEvent::Type )
{
}

void IGameEngineApp::onKeyBoardEvent( const LOGLW::KeyboardState& )
{
}

void IGameEngineApp::onMouseEvent( const LOGLW::MouseData& )
{
}

void IGameEngineApp::close()
{
    m_runLogicThread = false;
    m_logicThread.join();
    m_oglw->stopRenderingLoop();
    m_sdlw->stopEventLoop();
}

void IGameEngineApp::setLogicThreadSleep( int sleepInMs )
{
    m_logicThreadSleepMs = sleepInMs;
}

IGameEngineApp::~IGameEngineApp()
{
}
