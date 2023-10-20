#include "RunOnRenderThread.hpp"

#include "gameengine/IGameEngine.hpp"
#include "gameengine/IRenderDevice.hpp"

#include "CUL/CULInterface.hpp"
#include "CUL/Threading/ThreadUtil.hpp"

LOGLW::RunOnRenderThread::RunOnRenderThread()
{
    m_gameEngine = IGameEngine::getInstance();
    m_device = m_gameEngine->getDevice();
}

void LOGLW::RunOnRenderThread::Run( const std::function<void( void )> inFunction )
{
    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        inFunction();
    }
    else
    {
        m_gameEngine->addPreRenderTask(
            [this, inFunction]()
            {
                inFunction();
            } );
    }
}

LOGLW::RunOnRenderThread::~RunOnRenderThread()
{
}