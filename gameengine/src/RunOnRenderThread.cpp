#include "RunOnRenderThread.hpp"

#include "gameengine/IGameEngine.hpp"
#include "gameengine/IRenderDevice.hpp"

#include "CUL/CULInterface.hpp"
#include "CUL/Threading/ThreadUtil.hpp"

LOGLW::RunOnRenderThread::RunOnRenderThread()
{
    m_gameEngine = IGameEngine::getInstance();
    m_device = m_gameEngine->getDevice();

    m_threadUtil = &CUL::CULInterface::getInstance()->getThreadUtils();
    m_renderThreadId = CUL::CULInterface::getInstance()->getThreadUtils().getThreadId( "RenderThread" );
}

void LOGLW::RunOnRenderThread::Run( const std::function<void( void )> inFunction )
{
    if( getIsRenderThread() )
    {
        inFunction();
    }
    else
    {
        m_gameEngine->addPreRenderTask(
            [/*this,*/ inFunction]()
            {
                inFunction();
            } );
    }
}

bool LOGLW::RunOnRenderThread::getIsRenderThread() const
{
    return m_threadUtil->getCurrentThreadId() == m_renderThreadId;
}

LOGLW::RunOnRenderThread::~RunOnRenderThread()
{
}