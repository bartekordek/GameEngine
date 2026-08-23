#pragma once

#include "gameengine/Import.hpp"
#include <CUL/String/StringWrapper.hpp>

NAMESPACE_BEGIN( LOGLW )
class GAME_ENGINE_API IComponent
{
public:
    IComponent();
    virtual const CUL::StringWr& getName() const = 0;

#if !CUL_RELEASE && !CUL_RELWITHDEBINFO
    virtual void drawDebug() = 0;
#endif // #if !CUL_RELEASE && !CUL_RELWITHDEBINFO

    virtual ~IComponent();
protected:
private:
    // Deleted:
    IComponent( const IComponent& arg ) = delete;
    IComponent( IComponent&& arg ) = delete;
    IComponent& operator=( const IComponent& rhv ) = delete;
    IComponent& operator=( IComponent&& rhv ) = delete;
};
NAMESPACE_END( LOGLW )