#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"

#include "CUL/Graphics/Color.hpp"
#include "CUL/Math/Primitives/Triangle.hpp"

#include "CUL/STL_IMPORTS/STD_atomic.hpp"

NAMESPACE_BEGIN( LOGLW )

class Camera;
class IGameEngine;
class TransformComponent;
class Program;
class ShaderProgram;
class VertexArray;

class Triangle final: public IUtilityUser, public IObject
{
public:
    GAME_ENGINE_API Triangle( Camera& camera, IObject* parent, bool forceLegacy );

    GAME_ENGINE_API void setColor( const CUL::Graphics::ColorS& color );
    GAME_ENGINE_API ~Triangle();

protected:
    GAME_ENGINE_API void onNameChange( const String& newName ) override;

private:
    void render() override;
    void init();
    void createBuffers();
    void createShaders();
    void setColorAndTransformation();
    void release();
    void deleteBuffers();
    void setSize( const glm::vec3& size );

    CUL::MATH::Primitives::Triangle m_shape;
    glm::mat4 m_model;

    TransformComponent* m_transformComponent = nullptr;
    std::atomic<bool> m_recreateBuffers = false;

    Camera& m_camera;

    CUL::Graphics::ColorS m_color;
    bool m_unbindBuffersAfterDraw{ false };

    // Deleted
private:
    Triangle( Triangle& ) = delete;
    Triangle( Triangle&& ) = delete;
    Triangle& operator=( const Triangle& ) = delete;
    Triangle& operator=( const Triangle&& ) = delete;
};

NAMESPACE_END( LOGLW )