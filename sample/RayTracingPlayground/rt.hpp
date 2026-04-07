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

using Vec3f = glm::vec3;



class Sphere
{
public:
    void set_properties( const Vec3f& c, const float& r, std::size_t pointsCount );
    bool ray_intersect( const Vec3f& orig, const Vec3f& dir, float& t0 ) const;
    std::size_t getPointsCount() const;
    glm::vec3 getPoint( std::size_t i ) const;
    const Vec3f& getCenter() const
    {
        return m_center;
    }

    void move( const Vec3f& delta )
    {
        m_center += delta;
    }

    void resize( float delta )
    {
        m_radius *= delta;
    }

    float getRadius() const
    {
        return m_radius;
    }

protected:
private:
    Vec3f m_center;
    float m_radius;
    std::size_t m_pointsCount;
};

Vec3f cast_ray( const Vec3f& orig, const Vec3f& dir, const Sphere& sphere );

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

    Sphere m_sphere;

    glm::vec3 g_eyePos;

    std::unique_ptr<CUL::ITimer> m_timer;

    float m_time = 0.f;
    LOGLW::WinData m_winData;
    LOGLW::IWidgetEditable* m_widget{ nullptr };
};