#include "main.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/Windowing/IWindow.hpp"
#include "LOGLWAdditionalDeps/ImportImgui.hpp"

int main( int  // argc
          ,
          char**  // args
)
{
    CApp app( false, 1920, 1080, 256, 127, "TempalteApp", "" );
    app.run();

    return 0;
}

CApp::CApp( bool fullscreen, unsigned width, unsigned height, int x, int y, const char* winName, const char* configPath ):
    LOGLW::IGameEngineApp( fullscreen, width, height, x, y, winName, configPath, false )
{
}

void CApp::onInit()
{
    m_oglw->toggleDrawDebugInfo( true );
    m_oglw->drawDebugInfo( true );
    m_oglw->guiFrameDelegate.addDelegate(
        [this]( float x, float y )
        {
            guiIteration( x, y );
        } );
}

void CApp::customFrame()
{
}

void CApp::guiIteration( float x, float /*y*/ )
{
    CUL::String name = "MAIN";
    ImGui::Begin( name.cStr(), nullptr,
                  ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar );

    ImGui::SetWindowPos( { x, 0 } );

    if( ImGui::Button( "Test button!" ) )
    {
        m_oglw->getLoger()->logVariable( CUL::LOG::Severity::Warn, "Button pushed!" );
    }

    ImGui::End();
}

void CApp::customLogicThreadFrame( LOGLW::DurationType )
{
}

CApp::~CApp()
{
}
