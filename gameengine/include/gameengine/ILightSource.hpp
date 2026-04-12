#pragma once

#include "gameengine/Import.hpp"
#include "CUL/GenericUtils/NonCopyable.hpp"
#include "CUL/STL_IMPORTS/STD_set.hpp"
#include "CUL/STL_IMPORTS/STD_mutex.hpp"

namespace LOGLW
{

class IRenderable;
class ILightSource
{
public:
    GAME_ENGINE_API ILightSource();
    GAME_ENGINE_API void addObject( IRenderable* inObject );
    GAME_ENGINE_API void removeObject( IRenderable* inObject );
    GAME_ENGINE_API virtual ~ILightSource();

    CUL_NONCOPYABLE( ILightSource )
protected:
    GAME_ENGINE_API void broadcastLightSourcePositionChange( float inX, float inY, float inZ );

private:
    std::set<IRenderable*> m_attachedObjects;
    std::mutex m_attachedObjectsMtx;
};
}  // namespace LOGLW