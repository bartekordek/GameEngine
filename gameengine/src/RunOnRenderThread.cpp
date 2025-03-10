#include "RunOnRenderThread.hpp"

#include "gameengine/IGameEngine.hpp"
#include "gameengine/IRenderDevice.hpp"

#include "CUL/CULInterface.hpp"
#include "CUL/Threading/ThreadUtil.hpp"
#include "CUL/STL_IMPORTS/STD_condition_variable.hpp"

using namespace LOGLW;

RunOnRenderThread::RunOnRenderThread()
{
    m_gameEngine = IGameEngine::getInstance();
    m_device = m_gameEngine->getDevice();

    m_threadUtil = &CUL::CULInterface::getInstance()->getThreadUtils();
    m_renderThreadId = CUL::CULInterface::getInstance()->getThreadUtils().getThreadId( "RenderThread" );
}

void RunOnRenderThread::Run( const std::function<void( void )> inFunction )
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

void RunOnRenderThread::RunWaitForResult( const std::function<void( void )> inFunction )
{
    if( getIsRenderThread() )
    {
        inFunction();
    }
    else
    {
        std::condition_variable cv;
        std::mutex cv_m;
        bool taskCompleted{ false };

        m_gameEngine->addPreRenderTask(
            [/*this,*/ inFunction, &taskCompleted, &cv, &cv_m]()
            {
                inFunction();
                cv.notify_all();
                {
                    std::lock_guard<std::mutex> lk( cv_m );
                    taskCompleted = true;
                }
                cv.notify_all();
            } );

        std::unique_lock<std::mutex> lk( cv_m );
        cv.wait( lk,
                 [&taskCompleted]
                 {
                     return taskCompleted;
                 } );
    }
}

bool RunOnRenderThread::getIsRenderThread() const
{
    return m_threadUtil->getCurrentThreadId() == m_renderThreadId;
}

RunOnRenderThread::~RunOnRenderThread()
{
}