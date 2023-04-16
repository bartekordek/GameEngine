#include "gameengine/Primitives/Anchor.hpp"

using namespace LOGLW;

Anchor::Anchor( Camera& camera, IGameEngine& engine, IObject* parent, bool forceLegacy )
    : IObject( &engine, forceLegacy ), m_camera( camera ), m_engine( engine )
{
}

void Anchor::render()
{
    const auto children = getChildren();
    for( const auto child : children )
    {
        child->render();
    }
}