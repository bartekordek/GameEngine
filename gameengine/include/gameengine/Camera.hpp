#pragma once

#include "gameengine/Import.hpp"

#include "SDL2Wrapper/IWindow.hpp"

#include "CUL/GenericUtils/ValueChangeHook.hpp"

#include "CUL/IMPORT_GLM.hpp"
#include "CUL/IMPORT_GLM.hpp"

NAMESPACE_BEGIN( LOGLW )

using WindowSize = SDL2W::WindowSize;

class ProjectionData;

enum class ProjectionType : char
{
    UNSET = -1,
    ORTO = 0,
    PERSPECTIVE
};

class GAME_ENGINE_API Camera final
{
public:
    Camera();

    float m_fov = 90.f;
    float m_aspectRatio = 16.f / 9.f;
    glm::vec3 m_pos = { 0.f, 0.f, 0.f };
    glm::vec3 m_up = { 0.f, 1.f, 0.f };
    WindowSize m_size;
    float m_zNear = 64.0f;
    float m_left = 0.0f;
    float m_right = 0.0f;
    float m_top = 0.0f;
    float m_bottom = 0.0f;
    ProjectionType m_projectionType = ProjectionType::PERSPECTIVE;

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();

    float getLeft() const;
    float getRight() const;
    float getTop() const;
    float getBottom() const;
    float getZnear() const;
    float getZfar() const;
    float getFov() const;
    float getAspectRatio() const;

    const glm::vec3& getCenter() const;
    glm::vec3& getCenter();

    const glm::vec3& getEye() const;
    glm::vec3& getEye();

    const glm::vec3& getUp() const;

    void setUp( const glm::vec3& pos );
    void setZnear( float val );
    void setFov( float val );
    void setSize( const WindowSize& winSize );
    void setEyePos( const glm::vec3& pos );
    void setCenter( const glm::vec3& pos );
    void toggleDepthTest( bool enable );
    bool getDepthTestIsEnabled() const;

    ~Camera();

protected:
private:
    bool m_depthTest = true;
    glm::vec3 m_target = { 0.f, 0.f, -255.f };

    Camera( const Camera& arg ) = delete;
    Camera( Camera&& arg ) = delete;
    Camera& operator=( const Camera& rhv ) = delete;
    Camera& operator=( Camera&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )