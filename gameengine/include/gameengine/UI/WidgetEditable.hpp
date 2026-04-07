#pragma once

#include "gameengine/UI/Widget.hpp"

namespace LOGLW
{
struct S_RGBA_F;
struct S_RGBA_I;

class IWidgetEditable: public IWidget
{
public:
    GAME_ENGINE_API IWidgetEditable() = default;
    GAME_ENGINE_API virtual void updatePixel( std::size_t x, std::size_t y, const S_RGBA_I& color ) = 0;
    GAME_ENGINE_API virtual void updatePixel( std::size_t x, std::size_t y, const S_RGBA_F& color ) = 0;
    GAME_ENGINE_API virtual ~IWidgetEditable() = default;

    CUL_NONCOPYABLE( IWidgetEditable )
protected:
private:
};
}  // namespace LOGLW