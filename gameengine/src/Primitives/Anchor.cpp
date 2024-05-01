#include "gameengine/Primitives/Anchor.hpp"
#include "gameengine/Components/TransformComponent.hpp"

using namespace LOGLW;

Anchor::Anchor( Camera& camera, IGameEngine& engine, IObject* /*parent*/, bool forceLegacy )
    : IObject( "Anchor", &engine, forceLegacy ),
    m_camera( camera ), m_engine( engine )
{
    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
}

void Anchor::render()
{
    const auto children = getChildren();
    for( const auto child : children )
    {
        child->render();
    }
}