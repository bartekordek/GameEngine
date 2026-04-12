#pragma once

#include "gameengine/Scene/Scene.hpp"
#include "CUL/STL_IMPORTS/STD_unordered_map.hpp"
#include "CUL/STL_IMPORTS/STD_memory.hpp"
#include "CUL/STL_IMPORTS/STD_mutex.hpp"

namespace LOGLW
{
class SceneConcrete: public IScene
{
public:
    SceneConcrete();
    ~SceneConcrete();

    CUL_NONCOPYABLE( SceneConcrete )
protected:
private:
    Quad* createQuad( IObject* inParent ) override;
    PointLight* createPointLight( IObject* inParent ) override;
    void addObject( IRenderable* inObject ) override;
    void addLightSource( ILightSource* inLightSource ) override;
    void attachToEveryLightSource( IRenderable* inObject ) override;

    std::mutex m_objectsMtx;
    std::unordered_map<IRenderable*, std::unique_ptr<IRenderable>> m_objects;

    std::mutex m_lightSourcesMtx;
    std::unordered_map<ILightSource*, std::unique_ptr<ILightSource>> m_lightSources;
};
}  // namespace LOGLW