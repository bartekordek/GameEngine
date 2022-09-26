#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"

#include "CUL/STL_IMPORTS/STD_atomic.hpp"

NAMESPACE_BEGIN( LOGLW )

class Camera;
class IGameEngine;
class TransformComponent;

class Triangle: public IObject, public IUtilityUser
{
public:
    GAME_ENGINE_API Triangle( Camera& camera, IGameEngine& engine, IObject* parent  );

    GAME_ENGINE_API void setValues( const TriangleData& values );
    GAME_ENGINE_API void setColor( const TriangleColors& colors );
    GAME_ENGINE_API void setColor( const ColorS& color );

    GAME_ENGINE_API virtual ~Triangle();

protected:
private:
    void init();
    void render();
    void createBuffers();
    void createShaders();
    void setTransformation();
    void deleteBuffers();
    void applyColor();
    void release();

    TriangleCUL m_triangleMath;
    TransformComponent* m_transformComponent = nullptr;
    std::atomic<bool> m_recreateBuffers = false;
    class Program* m_shaderProgram = nullptr;
    class VertexArray* m_vao = nullptr;

    Camera& m_camera;
    IGameEngine& m_engine;

    TriangleData m_data;
    TriangleColors m_triangleColors;
    CUL::Graphics::ColorS m_color;

    Triangle( const Triangle& args ) = delete;
    Triangle( Triangle&& args ) = delete;
    Triangle& operator=( const Triangle& rhv ) = delete;
    Triangle& operator=( Triangle&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )