#pragma once

#include "gameengine/Import.hpp"

#include <CUL/String/StringWrapper.hpp>

#include "CUL/STL_IMPORTS/STD_map.hpp"

NAMESPACE_BEGIN( LOGLW )
using KeyboardState = std::map<CUL::StringWr, bool>;
NAMESPACE_END( LOGLW )