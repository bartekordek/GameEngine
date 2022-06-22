#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"

#include "CUL/Graphics/Color.hpp"

NAMESPACE_BEGIN( LOGLW )

class Camera;
class IGameEngine;
class TransformComponent;

class GAME_ENGINE_API Quad final: public IUtilityUser, public IObject
{
public:
    Quad( Camera& camera, IGameEngine& engine, IObject* parent );

    void setColor( const CUL::Graphics::ColorS& color );

    ~Quad();
protected:
private:
    void render() override;
    void init();
    void setTransformation();
    void applyColor();
    void release();

    TransformComponent* m_transformComponent = nullptr;

    class Program* m_shaderProgram = nullptr;
    class VertexArray* m_vao = nullptr;

    Camera& m_camera;
    IGameEngine& m_engine;

    CUL::Graphics::ColorS m_color;
};

NAMESPACE_END( LOGLW )