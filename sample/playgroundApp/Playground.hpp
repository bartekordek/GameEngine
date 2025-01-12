#pragma once

#include "SDL2Wrapper/Input/IKeyboardObserver.hpp"
#include "SDL2Wrapper/Input/IMouseObserver.hpp"

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
	class KeyboardState;
	class Camera;
	class Triangle;
	class Sprite;
	class Cube;
	class Quad;
}

namespace SDL2W
{
	class MouseData;
	class IWindow;
	namespace WindowEvent
	{
		enum class Type: short;
	}
}

class Playground final: public SDL2W::IMouseObserver, public SDL2W::IKeyboardObserver
{
public:
    Playground( std::int16_t w, std::int16_t h, std::int16_t x, std::int16_t y );
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
    void onMouseEvent( const SDL2W::MouseData& mouseData );
    void onKeyBoardEvent( const SDL2W::KeyboardState& key );
    void onWindowEvent( const SDL2W::WindowEvent::Type type );
    void timer();

    LOGLW::IGameEngine* m_engine = nullptr;
    SDL2W::IWindow* m_mainWindow = nullptr;
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
    std::int16_t m_width;
    std::int16_t m_height;
    std::int16_t m_x;
    std::int16_t m_y;
};