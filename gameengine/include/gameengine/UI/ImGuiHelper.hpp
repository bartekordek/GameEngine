#pragma once

#include "gameengine/Import.hpp"
#include "CUL/STL_IMPORTS/STD_cstdint.hpp"

struct ID3D12DescriptorHeap;
struct ID3D12Device;
struct ImGuiContext;
struct SDL_Window;

namespace LOGLW
{
// GAME_ENGINE_API
class CImguiHelper
{
public:
    GAME_ENGINE_API static CImguiHelper& getInstance();
    GAME_ENGINE_API void initDX12( SDL_Window* inWindow, std::int32_t inFrameCount, ID3D12Device* inDevice,
                                   ID3D12DescriptorHeap* inDescriptorHeap );
    GAME_ENGINE_API void initOpenGL( SDL_Window* inWindow, void* inGLContext );
    GAME_ENGINE_API ImGuiContext* getContext() const;

protected:
private:
    CImguiHelper();
    ~CImguiHelper();
    void buildFontAtlasses();
    ImGuiContext* m_context{ nullptr };
};
}  // namespace LOGLW