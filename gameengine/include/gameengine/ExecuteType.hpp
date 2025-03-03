#pragma once

#include "gameengine/Import.hpp"
#include "CUL/STL_IMPORTS/STD_cstdint.hpp"

namespace LOGLW
{
enum class EExecuteType : std::int8_t
{
	None = -1,
	WaitForCompletion,
	ExecuteOnRenderThread,
	Now
};
}