#pragma once

#include "gameengine/Import.hpp"

NAMESPACE_BEGIN( LOGLW )

class IGameEngine;

class GAME_ENGINE_API IEngineUser
{
public:
    IEngineUser(IGameEngine* engine);

    IGameEngine* getEngine();

    virtual ~IEngineUser();

protected:
private:
    IGameEngine* m_engine = nullptr;

};

NAMESPACE_END( LOGLW )