#include "gameengine/UI/ImGuiHelper.hpp"
#include "LOGLWAdditionalDeps/ImportImgui.hpp"

namespace LOGLW
{
CImguiHelper& CImguiHelper::getInstance()
{
    static CImguiHelper s_instance;
    return s_instance;
}

CImguiHelper::CImguiHelper()
{
}

#if defined( CUL_WINDOWS )
void CImguiHelper::initDX12( SDL_Window* inWindow, std::int32_t inFrameCount, ID3D12Device* inDevice,
                             ID3D12DescriptorHeap* inDescriptorHeap )
{
    IMGUI_CHECKVERSION();
    m_context = ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForD3D( inWindow );
    ImGui_ImplDX12_Init( inDevice, inFrameCount, DXGI_FORMAT_R8G8B8A8_UNORM, inDescriptorHeap,
                         inDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), inDescriptorHeap->GetGPUDescriptorHandleForHeapStart() );
    buildFontAtlasses();
}
#endif  // #if defined(CUL_WINDOWS)

void CImguiHelper::initOpenGL( SDL_Window* inWindow, void* inGLContext )
{
    IMGUI_CHECKVERSION();
    m_context = ImGui::CreateContext();
    ImGui::StyleColorsDark();

    buildFontAtlasses();
    ImGui_ImplSDL2_InitForOpenGL( inWindow, inGLContext );

    ImGui_ImplOpenGL3_Init();
}

void CImguiHelper::buildFontAtlasses()
{
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig cfg;
    cfg.OversampleH = 2;
    cfg.OversampleV = 2;

    static const ImWchar ranges[] = {
        0x0020, 0x00FF,  // Basic Latin + Latin-1 Supplement
        0x0100, 0x017F,  // Latin Extended-A (Polish lives here)
        0x0020, 0x00FF,  // Latin
        0x3040, 0x309F,  // Hiragana
        0x30A0, 0x30FF,  // Katakana
        0x4E00, 0x9FFF,  // Kanji
        0,
    };

#if defined( CUL_WINDOWS )
    io.Fonts->AddFontFromFileTTF( "C:/Windows/Fonts/Arial.ttf", 16.0f, &cfg, ranges );
#else   // #if defined( CUL_WINDOWS )
    io.Fonts->AddFontFromFileTTF( "Media/OpenSans-Regular.ttf", 16.0f, &cfg, ranges );
#endif  // #if CUL_WINDOWS
}

CImguiHelper::~CImguiHelper()
{
}
}  // namespace LOGLW