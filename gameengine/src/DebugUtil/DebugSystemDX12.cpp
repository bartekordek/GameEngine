#include "DebugSystemDX12.hpp"

#if defined( GAME_ENGINE_WINDOWS )

#include "LOGLWAdditionalDeps/ImportImgui.hpp"
#include "DebugUtil/DebugSystemParams.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"
#include "gameengine/IMPORT_Windows.hpp"

using namespace LOGLW;


DebugSystemDX12::DebugSystemDX12()
{
}

void DebugSystemDX12::init( const DebugSystemParams& params )
{
    m_params = std::make_unique<DebugSystemParams>();
    *m_params = params;

    IMGUI_CHECKVERSION();
    m_imguiContext = ImGui::CreateContext();
    ImGui::StyleColorsDark();
}

void DebugSystemDX12::frame()
{
}

DebugSystemDX12::~DebugSystemDX12()
{
}

void DebugSystemDX12::init( const DebugSystemParams& params )
{
}

void DebugSystemDX12::frame()
{
}

void DebugSystemDX12::addRenderCallback( const std::function<void( void )> renderDebugCallback )
{
}

#endif  // GAME_ENGINE_WINDOWS