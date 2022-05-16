#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"

NAMESPACE_BEGIN( LOGLW )

class Camera;
class IGameEngine;
class TransformComponent;

class GAME_ENGINE_API Quad final: public IUtilityUser, public IObject
{
public:
    Quad( Camera& camera, IGameEngine& engine, IObject* parent );

    ~Quad();
protected:
private:
    void render() override;
    void init();
    void setTransformation();
    void release();

    TransformComponent* m_transformComponent = nullptr;

    class VertexArray* m_vao = nullptr;

    Camera& m_camera;
    IGameEngine& m_engine;
};

NAMESPACE_END( LOGLW )