#pragma once

#include "gameengine/IUtilityUser.hpp"
#include "gameengine/Program.hpp"

NAMESPACE_BEGIN( LOGLW )

class GAME_ENGINE_API IProgramFactory
{
public:
    IProgramFactory() = default;
    virtual ~IProgramFactory() = default;

    virtual Program* createProgram() = 0;

protected:
private:
    IProgramFactory( const IProgramFactory& arg ) = delete;
    IProgramFactory( IProgramFactory&& arg ) = delete;
    IProgramFactory& operator=( const IProgramFactory& rhv ) = delete;
    IProgramFactory& operator=( IProgramFactory&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )