#pragma once

#include "gameengine/Import.hpp"
#include "SDL2Wrapper/RendererTypes.hpp"
#include "CUL/STL_IMPORTS/STD_functional.hpp"

NAMESPACE_BEGIN( LOGLW )

struct DebugSystemParams;

class DebugSystemBase
{
public:
    static DebugSystemBase* create( SDL2W::RenderTypes::RendererType type );

    DebugSystemBase();
    virtual void init( const DebugSystemParams& params ) = 0;
    virtual void frame() = 0;
    virtual void addRenderCallback( const std::function<void( void )> renderDebugCallback ) = 0;
    virtual ~DebugSystemBase();

protected:
private:
    std::unique_ptr<DebugSystemParams> m_params;
};

NAMESPACE_END( LOGLW )