#pragma once

#include "gameengine/Import.hpp"
#include "CUL/GenericUtils/Singleton.hpp"
#include "CUL/STL_IMPORTS/STD_functional.hpp"
#include "CUL/STL_IMPORTS/STD_memory.hpp"
#include "CUL/STL_IMPORTS/STD_thread.hpp"

namespace CUL
{
class ThreadUtil;
}

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
    void RunWaitForResult( const std::function<void( void )> inFunction );
    bool getIsRenderThread() const;

protected:
private:
    RunOnRenderThread();
    ~RunOnRenderThread();

    CUL::ThreadUtil* m_threadUtil{ nullptr };

    std::thread::id m_renderThreadId;

    IGameEngine* m_gameEngine = nullptr;
    IRenderDevice* m_device = nullptr;
};

NAMESPACE_END(LOGLW)