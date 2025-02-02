#pragma once

#include "gameengine/Import.hpp"

NAMESPACE_BEGIN( LOGLW )

NAMESPACE_BEGIN( WindowEvent )

enum class Type : short
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

NAMESPACE_END( LOGLW )