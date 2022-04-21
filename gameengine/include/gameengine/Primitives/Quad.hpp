#pragma once

#include "gameengine/IObject.hpp"

NAMESPACE_BEGIN( LOGLW )

class Camera;
class IGameEngine;

class GAME_ENGINE_API Quad final: public IObject
{
public:
    Quad( Camera& camera, IGameEngine& engine );

    void setSize(float width, float height);

    ~Quad();
protected:
private:
    void render() override;
    void init();

    float m_width = 1.f;
    float m_height = 1.f;

    class VertexArray* m_vao = nullptr;

    Camera& m_camera;
    IGameEngine& m_engine;
};

NAMESPACE_END( LOGLW )