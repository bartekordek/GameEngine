#pragma once

#include "SDL2Wrapper/Import.hpp"

#include "CUL/String.hpp"

#include "CUL/STL_IMPORTS/STD_map.hpp"

NAMESPACE_BEGIN( SDL2W )
using KeyboardState = std::map<CUL::String, bool>;
NAMESPACE_END( SDL2W )