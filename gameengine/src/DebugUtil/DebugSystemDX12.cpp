#include "DebugSystemDX12.hpp"

#if defined( GAME_ENGINE_WINDOWS )

#include "LOGLWAdditionalDeps/ImportImgui.hpp"
#include "DebugUtil/DebugSystemParams.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"
#include "gameengine/IMPORT_Windows.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "DX12/DeviceDX12.hpp"

using namespace LOGLW;


DebugSystemDX12::DebugSystemDX12()
{
}

void DebugSystemDX12::init( const DebugSystemParams& params )
{
    m_params = std::make_unique<DebugSystemParams>();
    *m_params = params;
    m_device = dynamic_cast<DeviceDX12*>( params.RenderDevice );
    m_deviceNative = reinterpret_cast<ID3D12Device*>( m_params->D3D12DevicePtr );

    //IMGUI_CHECKVERSION();
    //m_imguiContext = ImGui::CreateContext();
    //ImGui::StyleColorsDark();

    //ImGui_ImplDX12_Init(
    //    m_deviceNative,
    //    params.RenderDevice->getFrameBufferCount(),
    //    DXGI_FORMAT_R8G8B8A8_UNORM,
    //    g_pd3dSrvDescHeap,
    //                     g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(), g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart() );
}

void DebugSystemDX12::frame()
{
}

DebugSystemDX12::~DebugSystemDX12()
{
}


void DebugSystemDX12::addRenderCallback( const std::function<void( void )> renderDebugCallback )
{
}

#endif  // GAME_ENGINE_WINDOWS