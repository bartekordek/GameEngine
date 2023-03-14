#pragma once

#include "gameengine/Import.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include "gameengine/IMPORT_Windows.hpp"

NAMESPACE_BEGIN( LOGLW )

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

struct CommandListParams;

class CommandListWrapper
{
public:
	void init( const CommandListParams& params );
protected:
private:
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12PipelineState> m_pso;
};

NAMESPACE_END( LOGLW )

#endif // GAME_ENGINE_WINDOWS