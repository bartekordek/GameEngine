#pragma once

#include "gameengine/UI/Widget.hpp"

namespace LOGLW
{
struct S_RGBA;

class IWidgetEditable: public IWidget
{
public:
    GAME_ENGINE_API IWidgetEditable() = default;
    GAME_ENGINE_API virtual void updatePixel( std::size_t x, std::size_t y, const S_RGBA& color ) = 0;
    GAME_ENGINE_API virtual ~IWidgetEditable() = default;

protected:
private:
};
}  // namespace LOGLW