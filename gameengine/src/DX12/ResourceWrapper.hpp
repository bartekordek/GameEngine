#pragma once

#include "gameengine/Import.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include "DX12/ImportWindows.hpp"

NAMESPACE_BEGIN( LOGLW )


struct ResourceWrapper
{
	ComPtr<ID3D12Resource> Resource;
	D3D12_RESOURCE_STATES State = D3D12_RESOURCE_STATE_COMMON;
};

NAMESPACE_END( LOGLW )

#endif // GAME_ENGINE_WINDOWS