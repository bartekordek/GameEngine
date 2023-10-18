#pragma once

#include "gameengine/Import.hpp"
#include "CUL/GenericUtils/Singleton.hpp"
#include "CUL/STL_IMPORTS/STD_functional.hpp"

NAMESPACE_BEGIN( LOGLW )

class IGameEngine;
class IRenderDevice;

class RunOnRenderThread final: public CUL::Singleton<RunOnRenderThread>
{
public:
    friend class CUL::Singleton<RunOnRenderThread>;
    RunOnRenderThread( const RunOnRenderThread& ) = delete;
    RunOnRenderThread( RunOnRenderThread&& ) = delete;
    RunOnRenderThread& operator=( const RunOnRenderThread& ) = delete;
    RunOnRenderThread& operator=( RunOnRenderThread&& ) = delete;

    void Run( const std::function<void( void )> inFunction );

protected:
private:
    RunOnRenderThread();
    ~RunOnRenderThread();

    IGameEngine* m_gameEngine = nullptr;
    IRenderDevice* m_device = nullptr;
};

NAMESPACE_END(LOGLW)