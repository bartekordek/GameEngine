#pragma once

#include "gameengine/IObject.hpp"

namespace LOGLW
{
class IWidget: public IObject
{
public:
    GAME_ENGINE_API IWidget();
    GAME_ENGINE_API virtual ~IWidget();

    CUL_NONCOPYABLE( IWidget )
protected:
private:
};
}  // namespace LOGLW