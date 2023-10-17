#pragma once

#include "gameengine/Import.hpp"
#include "SDL2Wrapper/RendererTypes.hpp"
#include "CUL/STL_IMPORTS/STD_functional.hpp"
#include "CUL/STL_IMPORTS/STD_memory.hpp"

NAMESPACE_BEGIN( LOGLW )

struct DebugSystemParams;

class DebugSystemBase
{
public:
    static DebugSystemBase* create( SDL2W::RenderTypes::RendererType type );

    DebugSystemBase() = default;
    DebugSystemBase( const DebugSystemBase& ) = delete;
    DebugSystemBase( DebugSystemBase&& ) = delete;

    DebugSystemBase& operator=( const DebugSystemBase& ) = delete;
    DebugSystemBase& operator=( DebugSystemBase&& ) = delete;

    virtual void init( const DebugSystemParams& params ) = 0;
    virtual void frame() = 0;
    virtual void addRenderCallback( const std::function<void( void )> renderDebugCallback ) = 0;
    virtual ~DebugSystemBase() = default;

protected:
private:
    std::unique_ptr<DebugSystemParams> m_params;
};

NAMESPACE_END( LOGLW )