#include "Scene/SceneStoreConcrete.hpp"
#include "gameengine/Scene/Scene.hpp"
#include "Scene/SceneConcrete.hpp"

namespace LOGLW
{
SceneStoreConcrete::SceneStoreConcrete()
{
}

IScene* SceneStoreConcrete::createScene( const char* inSceneName )
{
    IScene* result{ nullptr };

    std::unique_ptr<SceneConcrete> ptr = std::make_unique<SceneConcrete>();
    result = ptr.get();
    m_scenes[inSceneName] = std::move( ptr );

    return result;
}

void SceneStoreConcrete::destroyScene( IScene* /*scene*/ )
{

}

void SceneStoreConcrete::destroyScene( const char* /*inSceneName*/ )
{

}

SceneStoreConcrete::~SceneStoreConcrete()
{
}

}  // namespace LOGLW
