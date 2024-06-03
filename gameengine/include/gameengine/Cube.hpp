#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"

#include "CUL/Math/Rotation.hpp"
#include "CUL/Graphics/Color.hpp"
#include "CUL/Math/Point.hpp"

#include "CUL/STL_IMPORTS/STD_array.hpp"
#include "CUL/STL_IMPORTS/STD_mutex.hpp"

NAMESPACE_BEGIN( CUL )
class CULInterface;

NAMESPACE_BEGIN( Graphics )
class IImageLoader;
NAMESPACE_END( Graphics )

NAMESPACE_END( CUL )

NAMESPACE_BEGIN( LOGLW )

class Camera;
class IGameEngine;
class TransformComponent;
class ShaderProgram;
struct VertexData;

class Cube final: public IObject, public IUtilityUser
{
public:
    GAME_ENGINE_API Cube( Camera* camera, IGameEngine* engine, IObject* parent, bool forceLegacy );

    GAME_ENGINE_API void setImage( unsigned wallIndex, const CUL::FS::Path& imagePath, CUL::Graphics::IImageLoader* imageLoader );
    GAME_ENGINE_API void setColor( const CUL::Graphics::ColorS& color );

    GAME_ENGINE_API ~Cube();

protected:
private:
    void init();
    void createBuffers();
    void createShaders();
    void deleteBuffers();
    void render() override;
    void setSize( const glm::vec3& size );
    void setTransformation();
    void release();

    bool m_recreateBuffers = false;
    TransformComponent* m_transformComponent = nullptr;
    bool m_initialized = false;

    Camera* m_camera = nullptr;
    IGameEngine& m_engine;

    std::mutex m_renderMutex;
    ShaderProgram* m_shaderProgram = nullptr;
    CUL::Graphics::ColorS m_color;
    std::unique_ptr<VertexData> m_vertexData;

public:
    // Deleted:
    Cube( const Cube& arg ) = delete;
    Cube( Cube&& arg ) = delete;
    Cube& operator=( const Cube& rhv ) = delete;
    Cube& operator=( Cube&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )