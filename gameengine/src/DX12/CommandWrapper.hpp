#pragma once


#include "gameengine/Import.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include "gameengine/IMPORT_Windows.hpp"

NAMESPACE_BEGIN( LOGLW )

struct CommandWrapper
{
	void create( ID3D12Device2* device );
	void reset();
	Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineState;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> Allocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList;
};

NAMESPACE_END( LOGLW )

#endif // #if defined(GAME_ENGINE_WINDOWS)