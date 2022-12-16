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
	void run();
	void closeApp();
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

	LOGLW::Triangle* g_blueTriangle = nullptr;
	LOGLW::Triangle* g_whiteTriangle = nullptr;

	LOGLW::Triangle* g_redTriangle = nullptr;
	LOGLW::Triangle* g_yellowTriangle = nullptr;

	LOGLW::Triangle* g_triangle00 = nullptr;
	LOGLW::Triangle* g_triangle01 = nullptr;

	float blueTriangleZ = -1.0f;
	float redTriangleZ = 1.0f;

	LOGLW::Sprite* g_sprite = nullptr;
	LOGLW::Cube* g_cube = nullptr;

	CUL::MATH::Angle g_angle;
	CUL::MATH::Point g_cubePos;
	CUL::MATH::Rotation g_cubeRotation;

	glm::vec3 g_eyePos;

	std::unique_ptr<CUL::ITimer> m_timer;
};