#pragma once

#include "gameengine/Import.hpp"

#include "CUL/String.hpp"

#include "CUL/STL_IMPORTS/STD_map.hpp"

NAMESPACE_BEGIN( LOGLW )
using KeyboardState = std::map<CUL::String, bool>;
NAMESPACE_END( LOGLW )