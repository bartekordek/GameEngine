#pragma once

#include "gameengine/Import.hpp"
#include "CUL/STL_IMPORTS/STD_cstdint.hpp"

NAMESPACE_BEGIN( LOGLW )

struct GAME_ENGINE_API WinSize
{
    std::uint16_t W{ static_cast<std::uint16_t>( 0 ) };
    std::uint16_t H{ static_cast<std::uint16_t>( 0 ) };
};

NAMESPACE_END( LOGLW )