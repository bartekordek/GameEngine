#pragma once

#include "gameengine/Import.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include "gameengine/IMPORT_Windows.hpp"

NAMESPACE_BEGIN( LOGLW )

struct FramePerapreParams
{
	ID3D12PipelineState* PipelineState = nullptr;
	ID3D12RootSignature* RootSignature = nullptr;
	D3D12_VIEWPORT* Viewport = nullptr;
	D3D12_RECT* ScissorRect = nullptr;
};

NAMESPACE_END( LOGLW )

#endif // GAME_ENGINE_WINDOWS