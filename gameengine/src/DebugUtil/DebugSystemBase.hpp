#pragma once

#include "gameengine/Import.hpp"
#include "CUL/STL_IMPORTS/STD_functional.hpp"

NAMESPACE_BEGIN( LOGLW )

struct DebugSystemParams;

NAMESPACE_BEGIN(DebugSystem)
enum class RendererType: short
{
    NONE = 0,
    OPENGL_LEGACY,
    OPENGL_MODERN,
    DIRECTX_12
};
NAMESPACE_END(DebugSystem)

class DebugSystemBase
{
public:
    static DebugSystemBase* create( DebugSystem::RendererType type );

    DebugSystemBase();
    virtual void init( const DebugSystemParams& params ) = 0;
    virtual void frame() = 0;
    virtual void addRenderCallback( const std::function<void( void )> renderDebugCallback ) = 0;
    virtual ~DebugSystemBase();

protected:
private:
};

NAMESPACE_END( LOGLW )