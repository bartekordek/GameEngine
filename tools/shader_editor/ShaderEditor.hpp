#pragma once

#include "SDL2Wrapper/Input/IKeyboardObserver.hpp"
#include "SDL2Wrapper/Input/IMouseObserver.hpp"

#include "CUL/ITimer.hpp"
#include "CUL/Math/Angle.hpp"
#include "CUL/Math/Point.hpp"
#include "CUL/Math/Rotation.hpp"
#include "CUL/IMPORT_GLM.hpp"
#include "CUL/STL_IMPORTS/STD_memory.hpp"
#include "CUL/STL_IMPORTS/STD_unordered_map.hpp"

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

namespace LOGLW
{
class ShaderProgram;
class VertexArray;
}

struct EditorState;

class ShaderEditor final: public SDL2W::IMouseObserver, public SDL2W::IKeyboardObserver
{
public:
    ShaderEditor( std::int16_t w, std::int16_t h, std::int16_t x, std::int16_t y );
    void run();
    void closeApp();
    virtual ~ShaderEditor();

protected:
private:
    void afterInit();
    void onMouseEvent( const SDL2W::MouseData& mouseData ) override;
    void onKeyBoardEvent( const SDL2W::KeyboardState& key ) override;
    void onWindowEvent( const SDL2W::WindowEvent::Type type );
    void timer();
    void guiIteration( float x, float y );
    void drawEditor(float x, float y, float w, float h, const CUL::String& name);

    LOGLW::IGameEngine* m_engine{ nullptr };
    SDL2W::IWindow* m_mainWindow{ nullptr };
    LOGLW::Camera* m_camera{ nullptr };


    std::unique_ptr<CUL::ITimer> m_timer;

    float m_time{0.f};

    std::unordered_map<std::string, std::unique_ptr<EditorState>> m_editors;

    LOGLW::VertexArray* m_vao{ nullptr };

    std::int16_t m_width;
    std::int16_t m_height;
    std::int16_t m_x;
    std::int16_t m_y;
};
