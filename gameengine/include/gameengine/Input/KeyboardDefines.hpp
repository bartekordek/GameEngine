#pragma once


#include "gameengine/String.hpp"

#include "CUL/STL_IMPORTS/STD_map.hpp"

NAMESPACE_BEGIN( LOGLW )
using KeyboardState = std::map<String, bool>;
NAMESPACE_END( LOGLW )