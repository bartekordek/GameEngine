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
class Camera;
class IGameEngine;
class IWidgetEditable;
class MouseData;
class IWindow;
class CSphere;
class PointLight;
}  // namespace LOGLW

namespace LOGLW
{

namespace WindowEvent
{
enum class Type : short;
}
}  // namespace LOGLW

using Vec3f = glm::vec3;

class RT_Playground final: public LOGLW::IMouseObserver, public LOGLW::IKeyboardObserver
{
public:
    RT_Playground( const LOGLW::WinData& inWinData );
    RT_Playground( const RT_Playground& ) = delete;
    RT_Playground( RT_Playground&& ) = delete;
    RT_Playground& operator=( const RT_Playground& ) = delete;
    RT_Playground& operator=( RT_Playground&& ) = delete;

    void run();
    void closeApp();
    ~RT_Playground() = default;

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

    LOGLW::CSphere* m_sphere{ nullptr };
    LOGLW::PointLight* m_pointLight{ nullptr };

    glm::vec3 g_eyePos;

    std::unique_ptr<CUL::ITimer> m_timer;

    float m_time = 0.f;
    LOGLW::WinData m_winData;
};