#include "gameengine/IGameEngineApp.hpp"
#include "gameengine/EngineParams.hpp"
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
}

void IGameEngineApp::init( const LOGLW::WinData& windowData, bool fullscreen, const char* configPath, bool legacy )
{
    LOGLW::EngineParams params;
    params.ConfigPath = configPath;
    params.WinDataVal = windowData;
    m_oglw.reset( LOGLW::IGameEngine::createGameEngine( params ) );
    m_logger = m_oglw->getLoger();
    m_device = m_oglw->getDevice();
    m_oglw->registerKeyboardEventListener( this );
    m_oglw->registerMouseEventListener( this );
    m_oglw->registerWindowEventListener( this );

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
    m_oglw->runEventLoop();
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

void IGameEngineApp::onWindowEvent( const LOGLW::WindowEvent::Type inType )
{
    if( inType == LOGLW::WindowEvent::Type::CLOSE )
    {
        close();
    }
}

void IGameEngineApp::onKeyBoardEvent( const LOGLW::KeyboardState& )
{
}

void IGameEngineApp::onMouseEvent( const LOGLW::MouseData& )
{
}

void IGameEngineApp::customLogicThreadFrame( DurationType )
{
}

void IGameEngineApp::close()
{
    m_runLogicThread = false;
    m_logicThread.join();
    m_oglw->stopRenderingLoop();
    m_oglw->stopEventLoop();
}

void IGameEngineApp::setLogicThreadSleep( int sleepInMs )
{
    m_logicThreadSleepMs = sleepInMs;
}

IGameEngineApp::~IGameEngineApp()
{
}
