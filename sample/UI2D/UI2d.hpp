#pragma once

#include "gameengine/Input/IKeyboardObserver.hpp"
#include "gameengine/Input/IMouseObserver.hpp"
#include "gameengine/Windowing/WinData.hpp"

#include "CUL/ITimer.hpp"
#include "CUL/Math/Angle.hpp"
#include "CUL/Math/Point.hpp"
#include "CUL/Math/Rotation.hpp"
#include "CUL/IMPORT_GLM.hpp"
#include "CUL/STL_IMPORTS/STD_memory.hpp"

namespace CUL
{
class ITimer;
}

namespace LOGLW
{
class IGameEngine;
class Camera;
}  // namespace LOGLW

namespace LOGLW
{
class MouseData;
class IWindow;
namespace WindowEvent
{
enum class Type : short;
}
}  // namespace LOGLW

class UI2d final: public LOGLW::IMouseObserver, public LOGLW::IKeyboardObserver
{
public:
    UI2d( const LOGLW::WinData& inWinData );
    UI2d( const UI2d& ) = delete;
    UI2d( UI2d&& ) = delete;
    UI2d& operator=( const UI2d& ) = delete;
    UI2d& operator=( UI2d&& ) = delete;

    void run();
    void closeApp();
    ~UI2d() = default;

protected:
private:
    void afterInit();
    void onMouseEvent( const LOGLW::MouseData& mouseData );
    void onKeyBoardEvent( const LOGLW::KeyboardState& key );
    void onWindowEvent( const LOGLW::WindowEvent::Type type );
    void timer();

    LOGLW::IGameEngine* m_engine = nullptr;
    LOGLW::IWindow* m_mainWindow = nullptr;
    LOGLW::Camera* m_camera = nullptr;


    glm::vec3 g_eyePos;

    std::unique_ptr<CUL::ITimer> m_timer;

    float m_time = 0.f;
    LOGLW::WinData m_winData;
};