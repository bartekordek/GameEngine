#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"

NAMESPACE_BEGIN( LOGLW )

class Camera;
class IGameEngine;

class GAME_ENGINE_API Quad final: public IUtilityUser, public IObject
{
public:
    Quad( Camera& camera, IGameEngine& engine, IObject* parent );

    void setSize(float width, float height);

    ~Quad();
protected:
private:
    void render() override;
    void init();
    void setTransformation();
    void release();

    float m_width = 2.f;
    float m_height = 2.f;

    class VertexArray* m_vao = nullptr;

    Camera& m_camera;
    IGameEngine& m_engine;
};

NAMESPACE_END( LOGLW )