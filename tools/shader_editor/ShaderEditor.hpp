#pragma once

#include "sdl2wrapper/Input/IKeyboardObserver.hpp"
#include "sdl2wrapper/Input/IMouseObserver.hpp"

#include "CUL/ITimer.hpp"
#include "CUL/Math/Angle.hpp"
#include "CUL/Math/Point.hpp"
#include "CUL/Math/Rotation.hpp"
#include "CUL/Math/SphericalCoord.hpp"
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
class TransformComponent;
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

namespace LOGLW
{
class ShaderProgram;
class VertexArray;
}  // namespace LOGLW

struct EditorState;

enum class EShaderUnitState : std::uint8_t
{
    Empty = 0u,
    Loaded,
    Compiled,
    Error
};

class ShaderEditor final: public LOGLW::IMouseObserver, public LOGLW::IKeyboardObserver
{
public:
    ShaderEditor( std::int16_t w, std::int16_t h, std::int16_t x, std::int16_t y );
    void run();
    void closeApp();
    virtual ~ShaderEditor();

protected:
private:
    void afterInit();
    void onMouseEvent( const LOGLW::MouseData& mouseData ) override;
    bool m_mouseDown{ false };

    void onKeyBoardEvent( const LOGLW::KeyboardState& key ) override;
    void onWindowEvent( const LOGLW::WindowEvent::Type type );
    void timer();
    void guiIteration( float x, float y );
    void drawLeftWindow( float x, float y );
    void drawRightWindow( float x, float y );
    void drawEditor( float x, float y, float w, float h, const CUL::String& name );

    LOGLW::IGameEngine* m_engine{ nullptr };
    LOGLW::IWindow* m_mainWindow{ nullptr };
    LOGLW::Camera* m_camera{ nullptr };

    std::unique_ptr<CUL::ITimer> m_timer;

    float m_time{ 0.f };

    std::unordered_map<std::string, std::unique_ptr<EditorState>> m_editors;

    LOGLW::VertexArray* m_vao{ nullptr };
    std::unique_ptr<LOGLW::TransformComponent> m_transformComponent;

    std::int16_t m_width;
    std::int16_t m_height;
    std::int16_t m_x;
    std::int16_t m_y;

    glm::vec3 m_eye;

    CUL::MATH::SphericalCoord<float> m_cameraPosSp;
    std::int32_t m_lastMouseX{ 0 };
    std::int32_t m_lastMouseY{ 0 };
};
