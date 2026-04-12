#include "gameengine/ILightSource.hpp"
#include "gameengine/ExecuteType.hpp"
#include "gameengine/IObject.hpp"
#include "gameengine/IRenderable.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "CUL/IMPORT_GLM.hpp"

namespace LOGLW
{
ILightSource::ILightSource()
{
}

void ILightSource::addObject( IRenderable* inObject )
{
    std::lock_guard<std::mutex> lock( m_attachedObjectsMtx );
    m_attachedObjects.insert( inObject );
}

void ILightSource::removeObject( IRenderable* inObject )
{
    std::lock_guard<std::mutex> lock( m_attachedObjectsMtx );
    m_attachedObjects.erase( inObject );
}

void ILightSource::broadcastLightSourcePositionChange( float inX, float inY, float inZ )
{
    std::lock_guard<std::mutex> lock( m_attachedObjectsMtx );
    for( IRenderable* ro: m_attachedObjects )
    {
        ShaderProgram* sp = ro->getObject()->getProgram();
        sp->setUniform(
            EExecuteType::ExecuteOnRenderThread, "lightPos", glm::vec3( inX, inY, inZ ) );
    }
}

ILightSource::~ILightSource()
{
}

}  // namespace LOGLW