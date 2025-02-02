#pragma once

#include "gameengine/Import.hpp"

#include "gameengine/ISDL2Wrapper.hpp"
#include "gameengine/Input/IKeyboardObserver.hpp"
#include "gameengine/Input/IMouseObserver.hpp"
#include "gameengine/Events/IWindowEventObservable.hpp"
#include "gameengine/Events/IWindowEventListener.hpp"

#include "CUL/ITimer.hpp"

#include "CUL/STL_IMPORTS/STD_memory.hpp"
#include "CUL/STL_IMPORTS/STD_atomic.hpp"
#include "CUL/STL_IMPORTS/STD_thread.hpp"

NAMESPACE_BEGIN( LOGLW )

class IGameEngine;

using DurationType = float;

class GAME_ENGINE_API IGameEngineApp: private LOGLW::IWindowEventObserver, private LOGLW::IKeyboardObserver, private LOGLW::IMouseObserver
{
public:
    enum class WinOrientation : short
    {
        CENTER = 0
    };

    IGameEngineApp() = delete;
    IGameEngineApp( bool fullscreen, unsigned width, unsigned height, int x, int y, const char* winName, const char* configPath,
                    bool legacy );
    IGameEngineApp( bool fullscreen, unsigned width, unsigned height, WinOrientation orientation, const char* winName, const char* configPath,
                    bool legacy );
    IGameEngineApp( const IGameEngineApp& ) = delete;
    IGameEngineApp( IGameEngineApp&& ) = delete;
    IGameEngineApp& operator=( const IGameEngineApp& ) = delete;
    IGameEngineApp& operator=( IGameEngineApp&& ) = delete;

    void run();
    void close();

    void setLogicThreadSleep( int sleepInMs );

    virtual ~IGameEngineApp();

protected:
    class CUL::LOG::ILogger* m_logger = nullptr;
    class IRenderDevice* m_device = nullptr;

    std::unique_ptr<LOGLW::ISDL2Wrapper> m_sdlw;
    std::unique_ptr<IGameEngine> m_oglw;

    int m_logicThreadSleepMs = 4;

private:
    void init( const LOGLW::WinData& windowData, bool fullscreen, const char* configPath, bool legacy );
    void logicThread();

    // Override these to add own events handling.
    void onWindowEvent( const LOGLW::WindowEvent::Type e ) override;
    void onKeyBoardEvent( const LOGLW::KeyboardState& key ) override;
    void onMouseEvent( const LOGLW::MouseData& md ) override;
    virtual void onInit() {};
    virtual void customFrame() {};
    virtual void customLogicThreadFrame( DurationType ){};

    std::atomic<bool> m_runLogicThread = true;
    std::thread m_logicThread;
    std::unique_ptr<CUL::ITimer> m_logicTimer;
};

NAMESPACE_END( LOGLW )