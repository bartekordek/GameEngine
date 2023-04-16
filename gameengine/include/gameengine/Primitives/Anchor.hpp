#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtilityUser.hpp"

NAMESPACE_BEGIN( LOGLW )

class Camera;

class Anchor final: public IUtilityUser, public IObject
{
public:
    GAME_ENGINE_API Anchor( Camera& camera, IGameEngine& engine, IObject* parent, bool forceLegacy );

protected:
private:
    void render() override;

    Camera& m_camera;
    IGameEngine& m_engine;
};

NAMESPACE_END( LOGLW )