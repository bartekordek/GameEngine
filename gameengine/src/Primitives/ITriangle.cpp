#include "gameengine/Primitives/ITriangle.hpp"
#include "Primitives/TriangleImpl.hpp"

using namespace LOGLW;

ITriangle::ITriangle( IGameEngine* engine ) : IObject(engine)
{
}

ITriangle::~ITriangle()
{
}