#pragma once

#include "DebugUtil/DebugSystemBase.hpp"
#include "CUL/STL_IMPORTS/STD_memory.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"
#include "CUL/STL_IMPORTS/STD_mutex.hpp"

struct ImGuiContext;

NAMESPACE_BEGIN(LOGLW)

class DebugSystemDX12 final: public DebugSystemBase
{
public:
protected:
private:

	// Inherited via DebugSystemBase
	void init( const DebugSystemParams& params ) override;
	void frame() override;
	void addRenderCallback( const std::function<void( void )> renderDebugCallback ) override;

	std::atomic_bool m_initialized{ false };
    ImGuiContext* m_imguiContext = nullptr;
    std::unique_ptr<DebugSystemParams> m_params;
    std::mutex m_callbacksMtx;
    std::vector<std::function<void( void )>> m_callbacks;
};

NAMESPACE_END(END)