#pragma once

#include "gameengine/Import.hpp"
#include "CUL/GenericUtils/NonCopyable.hpp"

namespace LOGLW
{
class IWidget;

class IUIService
{
public:
    GAME_ENGINE_API IUIService() = default;

    GAME_ENGINE_API virtual IWidget* createWidget() = 0;

    GAME_ENGINE_API virtual ~IUIService() = default;
    CUL_NONCOPYABLE( IUIService )
protected:
private:
};
}