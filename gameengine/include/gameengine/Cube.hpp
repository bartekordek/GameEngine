#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"

#include "CUL/Math/Rotation.hpp"
#include "CUL/Graphics/Color.hpp"

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

class GAME_ENGINE_API Cube final: public IObject, public IUtilityUser
{
public:
    Cube( Camera* camera, IGameEngine* engine );

    void setImage(unsigned wallIndex, const CUL::FS::Path& imagePath, CUL::Graphics::IImageLoader* imageLoader);
    void setColor( const CUL::Graphics::ColorS& color );

    ~Cube();
protected:
private:
    void createPlaceHolders();

    void render() override;
    void renderModern();
    void renderLegacy();

    void release();

    TransformComponent* m_transformComponent = nullptr;
    bool m_initialized = false;

    Camera* m_camera = nullptr;
    IGameEngine* m_engine = nullptr;

    std::mutex m_renderMutex;

    std::array<ITransformable::Pos, 6> m_wallsPositions;
    std::array<class Quad*, 6> m_walls = {};
    std::array<CUL::MATH::Rotation, 6> m_rotations;

    CUL::Graphics::ColorS m_color;

    // Deleted:
    Cube( const Cube& arg ) = delete;
    Cube( Cube&& arg ) = delete;
    Cube& operator=( const Cube& rhv ) = delete;
    Cube& operator=( Cube&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )