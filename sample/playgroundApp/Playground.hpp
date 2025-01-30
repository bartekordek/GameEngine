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
class Triangle;
class Sprite;
class Cube;
class Quad;
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

class Playground final: public LOGLW::IMouseObserver, public LOGLW::IKeyboardObserver
{
public:
    Playground( const LOGLW::WinData& inWinData );
    Playground( const Playground& ) = delete;
    Playground( Playground&& ) = delete;
    Playground& operator=( const Playground& ) = delete;
    Playground& operator=( Playground&& ) = delete;

    void run();
    void closeApp();
    ~Playground() = default;

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

    LOGLW::Triangle* m_triangleModern = nullptr;
    LOGLW::Triangle* m_triangleLegacy = nullptr;

    CUL::MATH::Rotation m_triangleRotation;

    LOGLW::Quad* m_quadModern = nullptr;
    LOGLW::Quad* m_quadLegacy = nullptr;

    float blueTriangleZ = -1.0f;
    float redTriangleZ = 1.0f;

    LOGLW::Sprite* g_sprite = nullptr;
    LOGLW::Cube* m_cubeModern = nullptr;
    LOGLW::Cube* m_cubeLegacy = nullptr;

    CUL::MATH::Angle m_angle;
    CUL::MATH::Point g_cubePos;
    CUL::MATH::Rotation m_cubeRotation;

    glm::vec3 g_eyePos;

    std::unique_ptr<CUL::ITimer> m_timer;

    float m_time = 0.f;
    LOGLW::WinData m_winData;
};