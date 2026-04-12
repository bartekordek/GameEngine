#include "Scene/SceneConcrete.hpp"
#include "gameengine/ILightSource.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IRenderable.hpp"
#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Render/PointLight.hpp"

namespace LOGLW
{

SceneConcrete::SceneConcrete()
{
}

Quad* SceneConcrete::createQuad( IObject* parent )
{
    Quad* result{ nullptr };

    Camera& camera = IGameEngine::getInstance()->getCamera();
    std::unique_ptr<Quad> quad = std::make_unique<Quad>( camera, parent, false );
    result = quad.get();

    {
        std::lock_guard<std::mutex> lock( m_objectsMtx );
        m_objects[result] = std::move( quad );
    }
    attachToEveryLightSource( result );

    return result;
}

PointLight* SceneConcrete::createPointLight( IObject* inParent )
{
    PointLight* result{ nullptr };

    std::unique_ptr<PointLight> light = std::make_unique<PointLight>( inParent );
    {
        std::lock_guard<std::mutex> lock( m_lightSourcesMtx );
        result = light.get();
        m_lightSources[result] = std::move( light );
    }
    {
        std::lock_guard<std::mutex> lock( m_objectsMtx );
        for( auto& [renderable, _] : m_objects )
        {
            result->addObject( renderable );
        }
    }

    return result;
}

void SceneConcrete::addObject( IRenderable* inObject )
{
    std::unique_ptr<IRenderable> object( inObject );
    std::lock_guard<std::mutex> lock( m_objectsMtx );
    m_objects[inObject] = std::move( object );
}

void SceneConcrete::addLightSource( ILightSource* inLightSource )
{
    std::unique_ptr<ILightSource> lightSource;
    lightSource.reset( inLightSource );
    m_lightSources[inLightSource] = std::move( lightSource );
}

void SceneConcrete::attachToEveryLightSource( IRenderable* inObject )
{
    std::lock_guard<std::mutex> lock( m_lightSourcesMtx );
    for( auto& [lightSource, _] : m_lightSources )
    {
        lightSource->addObject( inObject );
    }
}

SceneConcrete::~SceneConcrete()
{
}

}  // namespace LOGLW