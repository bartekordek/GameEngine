#include "gameengine/Render/PixelFormats.hpp"

namespace LOGLW
{
S_RGBA_I convertToRGBAI( const S_RGBA_F& color )
{
    return S_RGBA_I{ static_cast<std::uint8_t>( color.Red * 255.f ), static_cast<std::uint8_t>( color.Green * 255.f ),
                     static_cast<std::uint8_t>( color.Blue * 255.f ), static_cast<std::uint8_t>( color.Alpha * 255.f ) };
}
}