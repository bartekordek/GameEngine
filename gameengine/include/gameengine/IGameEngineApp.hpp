#pragma once

#include "gameengine/IGameEngine.hpp"

#include "SDL2Wrapper/ISDL2Wrapper.hpp"
#include "SDL2Wrapper/Input/IKeyboardObserver.hpp"
#include "SDL2Wrapper/Input/IMouseObserver.hpp"

template <typename TYPE>
using Ptr = CUL::GUTILS::DumbPtr<TYPE>;

NAMESPACE_BEGIN(LOGLW)

class IGameEngine;

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

    virtual ~IGameEngineApp();
protected:
    LOGLW::IObjectFactory* m_objFactory = nullptr;
    LOGLW::IProgramFactory* m_programFactory = nullptr;
    LOGLW::IShaderFactory* m_shaderfactory = nullptr;
    CUL::LOG::ILogger* m_logger = nullptr;
    LOGLW::IUtility* m_gutil = nullptr;


    Ptr<SDL2W::ISDL2Wrapper> m_sdlw;
    Ptr<LOGLW::IGameEngine> m_oglw;
private:
    void init( const SDL2W::WindowData& windowData, bool fullscreen, const char* configPath, bool legacy );
    void logicThread();

    // Override these to add own events handling.
    virtual void onWindowEvent(const SDL2W::WindowEvent::Type e) override;
    virtual void onKeyBoardEvent(const SDL2W::IKey& key) override;
    virtual void onMouseEvent(const SDL2W::MouseData& md) override;
    virtual void onInit() {};
    virtual void customFrame() {};
    virtual void customLogicThreadFrame(){};

    std::atomic<bool> m_runLogicThread = true;
    std::thread m_logicThread;

};

NAMESPACE_END( LOGLW )
