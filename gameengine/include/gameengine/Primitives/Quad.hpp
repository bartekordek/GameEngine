#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"
#include "gameengine/AttributeMeta.hpp"

#include "CUL/Graphics/Color.hpp"
#include "CUL/IRegisteredObject.hpp"
#include "CUL/Math/Primitives/Quad.hpp"

#include "CUL/STL_IMPORTS/STD_atomic.hpp"

NAMESPACE_BEGIN( LOGLW )

class Camera;
class IGameEngine;
class TransformComponent;
class VertexArray;

class Quad final:
    public IUtilityUser,
    public IObject,
    public CUL::IRegisterdObject
{
public:
    GAME_ENGINE_API Quad( Camera& camera, IGameEngine& engine, IObject* parent, bool forceLegacy );

    GAME_ENGINE_API void setColor( const CUL::Graphics::ColorS& color );

    GAME_ENGINE_API ~Quad();

protected:
    void onNameChange( const CUL::String& newName ) override;

private:
    void render() override;
    void init();
    void createBuffers();
    void createShaders();
    void setTransformationAndColor();
    void release();
    void updateBuffers();
    void updateBuffers_impl();
    void setSize( const glm::vec3& size );

    CUL::MATH::Primitives::Quad m_shape;
    glm::mat4 m_model;
    VertexData m_vboData;

    TransformComponent* m_transformComponent{ nullptr };

    IGameEngine& m_engine;

    CUL::Graphics::ColorS m_color;
    bool m_unbindBuffersAfterDraw{ false };

// Deleted
private:
    Quad( Quad& ) = delete;
    Quad( Quad&& ) = delete;
    Quad& operator=( const Quad& ) = delete;
    Quad& operator=( const Quad&& ) = delete;
};

NAMESPACE_END( LOGLW )