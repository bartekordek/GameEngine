#pragma once

#include "SDL2Wrapper/Import.hpp"

NAMESPACE_BEGIN( SDL2W )

NAMESPACE_BEGIN( WindowEvent )

enum class Type: short
{
    NONE = 0,
    CLOSE,
    MINIMIZED,
    RESTORED,
    MOVED,
    RESIZE,
    MOUSE_ENTERED,
    MOUSE_LEAVED,
    FOCUS_GAINED,
    FOCUS_LEAVED
};

NAMESPACE_END( WindowEvent )

NAMESPACE_END( SDL2W )