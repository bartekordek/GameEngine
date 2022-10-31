#include "gameengine/IGameEngineApp.hpp"
#include "gameengine/IGameEngine.hpp"

#include "SDL2Wrapper/IWindow.hpp"
#include "SDL2Wrapper/WindowData.hpp"

using namespace LOGLW;

IGameEngineApp::IGameEngineApp( bool fullscreen, unsigned width,
                                      unsigned height, int x, int y,
                                      const char* winName,
                                      const char* configPath, bool legacy )
{
    SDL2W::WindowData windowData;
    windowData.name = "Test";
    windowData.pos = { x, y };
    windowData.currentRes.setSize( width, height );
    windowData.rendererName = "opengl";
    windowData.name = winName;

    init( windowData, fullscreen, configPath, legacy );
}

IGameEngineApp::IGameEngineApp( bool fullscreen, unsigned width,
                                      unsigned height, WinPos pos,
                                      const char* winName,
                                      const char* configPath, bool legacy )
{
    SDL2W::WindowData windowData;
    windowData.name = "Test";
    windowData.currentRes.setSize( width, height );
    windowData.name = winName;

    init( windowData, fullscreen, configPath, legacy );

    if ( pos == LOGLW::IGameEngineApp::WinPos::CENTER )
    {
        auto screenSize =
            m_sdlw->getMainWindow()->getCurrentScreenNativeResolution();
        CUL::Graphics::Pos2Di newPos;
        newPos.setXY( ( screenSize.w - width ) / 2,
                      ( screenSize.h - height ) / 2 );
        m_sdlw->getMainWindow()->setPos( newPos );
    }
}

void IGameEngineApp::init( const SDL2W::WindowData& windowData,
                              bool fullscreen, const char* configPath, bool legacy )
{
    m_sdlw.reset( SDL2W::ISDL2Wrapper::createSDL2Wrapper() );
    m_sdlw->init( windowData, configPath );
    m_sdlw->registerWindowEventListener( this );
    m_sdlw->registerKeyboardEventListener( this );
    m_sdlw->registerMouseEventListener( this );

    m_oglw.reset( LOGLW::IGameEngine::createGameEngine( m_sdlw.get(), legacy ) );
    m_logger = m_oglw->getLoger();
    m_gutil = m_oglw->getDevice();

    m_oglw->onInitialize( [this]() {
        m_objFactory = m_oglw->getObjectFactory();
        onInit();
    } );
    m_oglw->beforeFrame( [this]() {
        customFrame();
    } );

    m_sdlw->getMainWindow()->setFullscreen( fullscreen );

    //LOGLW::ProjectionData g_projectionData;
    //const auto& winSize = m_sdlw->getMainWindow()->getSize();
    //g_projectionData.setSize( { winSize.getWidth(), winSize.getHeight() } );
    //g_projectionData.setZNear( 127.0f );
    //g_projectionData.setZfar( -64.0f );
    //g_projectionData.setCenter( { 0.f, 0.f } );
    //g_projectionData.setEyePos( { 0.f, 0.f, 128.f } );
    //g_projectionData.m_projectionType = LOGLW::ProjectionType::PERSPECTIVE;
    //m_oglw->setProjection( g_projectionData );


    m_logicTimer.reset( CUL::TimerFactory::getChronoTimer() );
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

void IGameEngineApp::onWindowEvent( const SDL2W::WindowEvent::Type )
{
}

void IGameEngineApp::onKeyBoardEvent( const SDL2W::KeyboardState& )
{
}

void IGameEngineApp::onMouseEvent( const SDL2W::MouseData& )
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
