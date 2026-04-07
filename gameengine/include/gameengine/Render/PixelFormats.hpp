#pragma once

#include "CUL/STL_IMPORTS/STD_inttypes.hpp"

namespace LOGLW
{
struct S_RGBA_I
{
    std::uint8_t Red{ 0u };
    std::uint8_t Green{ 0u };
    std::uint8_t Blue{ 0u };
    std::uint8_t Alpha{ 0u };
};

struct S_RGBA_F
{
    float Red{ 0.f };
    float Green{ 0.f };
    float Blue{ 0.f };
    float Alpha{ 0.f };
};

S_RGBA_I convertToRGBAI( const S_RGBA_F& color );

}  // namespace LOGLW