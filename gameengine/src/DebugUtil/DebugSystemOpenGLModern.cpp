#include "DebugUtil/DebugSystemOpenGLModern.hpp"

#include "LOGLWAdditionalDeps/ImportImgui.hpp"
#include "DebugUtil/DebugSystemParams.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"

using namespace LOGLW;

DebugSystemOpenGLModern::DebugSystemOpenGLModern()
{
}

void DebugSystemOpenGLModern::init( const DebugSystemParams& params )
{
    if( m_initialized )
    {
        CUL::Assert::simple( false, "Already initialized" );
    }

    m_params = std::make_unique< DebugSystemParams>();
    *m_params = params;

    IMGUI_CHECKVERSION();
    m_imguiContext = ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL( params.SDLWindow, params.SDL_GL_Context );

    ImGui_ImplOpenGL3_Init();
    m_initialized = true;

}

void DebugSystemOpenGLModern::frame()
{
    if( m_initialized )
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame( m_params->SDLWindow );
        ImGui::NewFrame();
        {
            std::lock_guard<std::mutex> locker( m_callbacksMtx );
            for( auto& callback : m_callbacks )
            {
                callback();
            }
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
    }
}

void DebugSystemOpenGLModern::addRenderCallback( const std::function<void( void )> renderDebugCallback )
{
    std::lock_guard<std::mutex> locker( m_callbacksMtx );
    m_callbacks.push_back( renderDebugCallback );
}

DebugSystemOpenGLModern::~DebugSystemOpenGLModern()
{
    if( m_initialized )
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }
}