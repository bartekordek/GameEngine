#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"

#include "CUL/Graphics/Color.hpp"
#include "CUL/IRegisteredObject.hpp"
#include "CUL/Math/Primitives/Quad.hpp"

#include "CUL/STL_IMPORTS/STD_atomic.hpp"

NAMESPACE_BEGIN( LOGLW )

class Camera;
class IGameEngine;
class TransformComponent;
class ShaderProgram;
class VertexArray;

class Quad final: public IUtilityUser, public IObject, public CUL::IRegisterdObject
{
public:
    GAME_ENGINE_API Quad( Camera& camera, IGameEngine& engine, IObject* parent, bool forceLegacy );

    GAME_ENGINE_API void setColor( const CUL::Graphics::ColorS& color );
    GAME_ENGINE_API ShaderProgram* getProgram() const;

    GAME_ENGINE_API ~Quad();

protected:
private:
    void render() override;
    void init();
    void createBuffers();
    void createShaders();
    void setTransformation();
    void applyColor();
    void release();
    void deleteBuffers();
    void setSize( const glm::vec3& size );

    CUL::MATH::Primitives::Quad m_shape;
    glm::mat4 m_model;

    TransformComponent* m_transformComponent = nullptr;
    std::atomic<bool> m_recreateBuffers = true;
    ShaderProgram* m_shaderProgram = nullptr;
    VertexArray* m_vao = nullptr;

    Camera& m_camera;
    IGameEngine& m_engine;

    CUL::Graphics::ColorS m_color;

// Deleted
private:
    Quad( Quad& ) = delete;
    Quad( Quad&& ) = delete;
    Quad& operator=( const Quad& ) = delete;
    Quad& operator=( const Quad&& ) = delete;
};

NAMESPACE_END( LOGLW )