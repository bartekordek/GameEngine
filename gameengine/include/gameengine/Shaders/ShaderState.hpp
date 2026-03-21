#pragma once

#include "gameengine/Defines.hpp"
#include "CUL/STL_IMPORTS/STD_cstdint.hpp"

namespace LOGLW
{
enum class EShaderUnitState : std::int8_t
{
    Unkown = 1,
    Loaded,
    Compiled,
    Unloaded,
    WaitingForResult,
    Error
};

struct SCompilationResult
{
    EShaderUnitState State{ EShaderUnitState::Unkown };
    String ErrorInfo;
};
}