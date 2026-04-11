#pragma once

#include "gameengine/Import.hpp"
#include "CUL/GenericUtils/NonCopyable.hpp"

namespace LOGLW
{
class CScene;
class IGameEngine;

class CSceneStore
{
public:
    GAME_ENGINE_API CScene* createScene( const char* inSceneName );
    GAME_ENGINE_API void destroyScene( CScene* scene );
    GAME_ENGINE_API void destroyScene( const char* inSceneName );
    CUL_NONCOPYABLE( CSceneStore )

protected:
private:
    friend class IGameEngine;

    CSceneStore( IGameEngine& engine );
    ~CSceneStore();

    IGameEngine& m_engine;
};
}  // namespace LOGLW