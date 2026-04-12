#pragma once

#include "gameengine/Import.hpp"
#include "CUL/GenericUtils/NonCopyable.hpp"

namespace LOGLW
{

class ILightSource;
class IObject;
class IRenderable;
class PointLight;
class Quad;

class IScene
{
public:
    GAME_ENGINE_API IScene();

    GAME_ENGINE_API virtual Quad* createQuad( IObject* inParent ) = 0;
    GAME_ENGINE_API virtual PointLight* createPointLight( IObject* inParent ) = 0;
    GAME_ENGINE_API virtual void addObject( IRenderable* inObject ) = 0;
    GAME_ENGINE_API virtual void addLightSource( ILightSource* inLightSource ) = 0;
    GAME_ENGINE_API virtual void attachToEveryLightSource( IRenderable* inObject ) = 0;
    GAME_ENGINE_API virtual ~IScene();

    CUL_NONCOPYABLE( IScene )
protected:
private:
};
}  // namespace LOGLW