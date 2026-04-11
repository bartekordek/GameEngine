#include "gameengine/Scene/SceneStore.hpp"
#include "gameengine/Scene/Scene.hpp"

namespace LOGLW
{
CSceneStore::CSceneStore( IGameEngine& engine ) : m_engine( engine )
{
}

CScene* CSceneStore::createScene( const char* inSceneName )
{
    return new CScene();
}

}  // namespace LOGLW
