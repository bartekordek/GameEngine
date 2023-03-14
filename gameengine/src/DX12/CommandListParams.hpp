#pragma once

#include "gameengine/Import.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include "gameengine/IMPORT_Windows.hpp"

NAMESPACE_BEGIN( LOGLW )

struct CommandListParams
{
	ID3D12Device2* Device = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC Description;
};

NAMESPACE_END( LOGLW )

#endif // GAME_ENGINE_WINDOWS