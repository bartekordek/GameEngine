#include "DX12/CommandListParams.hpp"

#if defined(GAME_ENGINE_WINDOWS)

using namespace LOGLW;

CommandListParams::CommandListParams()
{
	std::memset( &Description, 0, sizeof( Description ) );
}

#endif // #if defined(GAME_ENGINE_WINDOWS)