#pragma once

#include <gameengine/Import.hpp>
#include <CUL/GenericUtils/NonCopyable.hpp>

NAMESPACE_BEGIN( LOGLW )

class IGameEngine;

class GAME_ENGINE_API IEngineUser
{
public:
    IEngineUser();

    IGameEngine& getEngine();

    virtual ~IEngineUser();

    CUL_NONCOPYABLE( IEngineUser )
protected:
private:
    IGameEngine& m_engine;
};

NAMESPACE_END( LOGLW )