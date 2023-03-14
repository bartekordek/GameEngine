#pragma once

#include "gameengine/Import.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include "gameengine/IMPORT_Windows.hpp"

NAMESPACE_BEGIN( LOGLW )

struct PrimitiveInfo
{
	D3D12_PRIMITIVE_TOPOLOGY Topology;
	unsigned StartSlot = 0u;
	unsigned NumViews = 0u;
	D3D12_VERTEX_BUFFER_VIEW* VBV = nullptr;

	unsigned VertexCountPerInstance = 0u;
	unsigned InstanceCount = 0u;
	unsigned StartVertexLocation = 0u;
	unsigned StartInstanceLocation = 0u;
};

NAMESPACE_END( LOGLW )

#endif // GAME_ENGINE_WINDOWS