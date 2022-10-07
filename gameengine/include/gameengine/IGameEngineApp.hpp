#pragma once

#include "gameengine/Import.hpp"

#include "SDL2Wrapper/ISDL2Wrapper.hpp"
#include "SDL2Wrapper/Input/IKeyboardObserver.hpp"
#include "SDL2Wrapper/Input/IMouseObserver.hpp"
#include "SDL2Wrapper/IWindowEventObservable.hpp"
#include "SDL2Wrapper/IWindowEventListener.hpp"

#include "CUL/ITimer.hpp"

#include "CUL/STL_IMPORTS/STD_memory.hpp"
#include "CUL/STL_IMPORTS/STD_atomic.hpp"
#include "CUL/STL_IMPORTS/STD_thread.hpp"

NAMESPACE_BEGIN(LOGLW)

class IGameEngine;

using DurationType = float;

class GAME_ENGINE_API IGameEngineApp:
    private SDL2W::IWindowEventObserver,
    private SDL2W::IKeyboardObserver,
    private SDL2W::IMouseObserver
{
public:
    enum class WinPos : short
    {
        CENTER = 0
    };

    IGameEngineApp( bool fullscreen, unsigned width, unsigned height, int x, int y, const char* winName, const char* configPath, bool legacy );
    IGameEngineApp( bool fullscreen, unsigned width, unsigned height, WinPos pos, const char* winName, const char* configPath, bool legacy );

    void run();
    void close();

    void setLogicThreadSleep( int sleepInMs );

    virtual ~IGameEngineApp();
protected:
    class IObjectFactory* m_objFactory = nullptr;
    class CUL::LOG::ILogger* m_logger = nullptr;
    class IUtility* m_gutil = nullptr;

    std::unique_ptr<SDL2W::ISDL2Wrapper> m_sdlw;
    std::unique_ptr<IGameEngine> m_oglw;

    int m_logicThreadSleepMs = 4;

private:
    void init( const SDL2W::WindowData& windowData, bool fullscreen, const char* configPath, bool legacy );
    void logicThread();

    // Override these to add own events handling.
    virtual void onWindowEvent(const SDL2W::WindowEvent::Type e) override;
    virtual void onKeyBoardEvent(const SDL2W::KeyboardState& key) override;
    virtual void onMouseEvent(const SDL2W::MouseData& md) override;
    virtual void onInit() {};
    virtual void customFrame() {};
    virtual void customLogicThreadFrame( DurationType ){};

    std::atomic<bool> m_runLogicThread = true;
    std::thread m_logicThread;
    std::unique_ptr<CUL::ITimer> m_logicTimer;

// Deleted
private:
    IGameEngineApp( IGameEngineApp& ) = delete;
    IGameEngineApp( IGameEngineApp&& ) = delete;
    IGameEngineApp& operator=( const IGameEngineApp& ) = delete;
    IGameEngineApp& operator=( const IGameEngineApp&& ) = delete;
};

NAMESPACE_END( LOGLW )