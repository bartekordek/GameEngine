#include "gameengine/Primitives/Anchor.hpp"
#include "gameengine/Components/TransformComponent.hpp"

using namespace LOGLW;

Anchor::Anchor( Camera& camera, IObject* /*parent*/, bool forceLegacy ):
    IObject( "Anchor", forceLegacy ),
    m_camera( camera )
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