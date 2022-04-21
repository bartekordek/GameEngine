#pragma once

#include "gameengine/Import.hpp"

NAMESPACE_BEGIN( LOGLW )
class GAME_ENGINE_API IComponent
{
public:
    IComponent() = default;

    virtual ~IComponent() = default;
protected:
private:
    // Deleted:
    IComponent( const IComponent& arg ) = delete;
    IComponent( IComponent&& arg ) = delete;
    IComponent& operator=( const IComponent& rhv ) = delete;
    IComponent& operator=( IComponent&& rhv ) = delete;
};
NAMESPACE_END( LOGLW )