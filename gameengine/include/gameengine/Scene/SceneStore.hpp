#pragma once

#include "gameengine/Import.hpp"
#include "CUL/GenericUtils/NonCopyable.hpp"

namespace LOGLW
{
class IScene;

class ISceneStore
{
public:
    GAME_ENGINE_API ISceneStore();
    GAME_ENGINE_API virtual IScene* createScene( const char* inSceneName ) = 0;
    GAME_ENGINE_API virtual void destroyScene( IScene* scene ) = 0;
    GAME_ENGINE_API virtual void destroyScene( const char* inSceneName ) = 0;
    GAME_ENGINE_API virtual ~ISceneStore();
    CUL_NONCOPYABLE( ISceneStore )

protected:
private:
};
}  // namespace LOGLW