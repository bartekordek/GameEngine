#pragma once

#include "gameengine/Scene/SceneStore.hpp"
#include "CUL/STL_IMPORTS/STD_unordered_map.hpp"
#include "CUL/STL_IMPORTS/STD_string.hpp"
#include "CUL/STL_IMPORTS/STD_memory.hpp"

namespace LOGLW
{
class SceneStoreConcrete: public ISceneStore
{
public:
    SceneStoreConcrete();

    IScene* createScene( const char* inSceneName ) override;
    void destroyScene( IScene* scene ) override;
    void destroyScene( const char* inSceneName ) override;

    ~SceneStoreConcrete();

    CUL_NONCOPYABLE( SceneStoreConcrete )

protected:
private:
    std::unordered_map<std::string, std::unique_ptr<IScene>> m_scenes;
};
}  // namespace LOGLW
