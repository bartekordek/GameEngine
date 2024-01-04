#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"

NAMESPACE_BEGIN( LOGLW )

class Camera;

class Anchor final: public IUtilityUser, public IObject
{
public:
    GAME_ENGINE_API Anchor( Camera& camera, IGameEngine& engine, IObject* parent, bool forceLegacy );
    GAME_ENGINE_API Anchor( const Anchor& ) = delete;
    GAME_ENGINE_API Anchor( Anchor&& ) = delete;
    GAME_ENGINE_API Anchor& operator=( const Anchor& ) = delete;
    GAME_ENGINE_API Anchor& operator=( Anchor&& ) = delete;

protected:
private:
    void render() override;

    Camera& m_camera;
    IGameEngine& m_engine;

    TransformComponent* m_transformComponent = nullptr;
};

NAMESPACE_END( LOGLW )