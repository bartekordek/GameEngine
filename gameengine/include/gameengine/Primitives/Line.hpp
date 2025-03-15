#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"
#include "gameengine/IRenderDevice.hpp"

#include "CUL/Math/Primitives/Line.hpp"

#include "CUL/STL_IMPORTS/STD_atomic.hpp"

NAMESPACE_BEGIN( LOGLW )

class Camera;
class IGameEngine;
class TransformComponent;
class ShaderProgram;
struct VertexData;

class Line final: public IObject, public IUtilityUser
{
public:
    GAME_ENGINE_API Line( Camera& camera, IGameEngine& engine, IObject* parent, bool forceLegacy );

    GAME_ENGINE_API void setValues( const CUL::MATH::Primitives::Line& values );
    GAME_ENGINE_API void setColor( const ColorS& color );
    GAME_ENGINE_API void setLength( float length );

    GAME_ENGINE_API ~Line();

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

    CUL::MATH::Primitives::Line m_line;
    TransformComponent* m_transformComponent = nullptr;
    ShaderProgram* m_shaderProgram{ nullptr };
    std::unique_ptr<VertexData> m_vertexData;

    Camera& m_camera;
    IGameEngine& m_engine;
    CUL::Graphics::ColorS m_color;

    Line( const Line& args ) = delete;
    Line( Line&& args ) = delete;
    Line& operator=( const Line& rhv ) = delete;
    Line& operator=( Line&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )