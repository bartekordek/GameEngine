#include "main.hpp"
#include "gameengine/IGameEngine.hpp"


int main( int argc, char* args[] )
{
    CApp app( false, 1920, 1080, 256, 127, "TempalteApp", "" );
    app.run();

    return 0;
}

CApp::CApp( bool fullscreen, unsigned width, unsigned height, int x, int y, const char* winName, const char* configPath )
    : LOGLW::IGameEngineApp( fullscreen, width, height, x, y, winName, configPath, false )
{
}

void CApp::onInit()
{
    m_oglw->toggleDrawDebugInfo( true );
    m_oglw->drawDebugInfo( true );
}

void CApp::customFrame()
{
}

void CApp::customLogicThreadFrame( LOGLW::DurationType )
{
}

CApp::~CApp()
{
}
