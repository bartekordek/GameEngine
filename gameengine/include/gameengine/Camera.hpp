#pragma once

#include "gameengine/Import.hpp"

#include "CUL/GenericUtils/SimpleDelegate.hpp"

#include "CUL/IMPORT_GLM.hpp"
#include "CUL/STL_IMPORTS/STD_atomic.hpp"

NAMESPACE_BEGIN( LOGLW )

struct WindowSize
{
    int w = 0;
    int h = 0;
};

class ProjectionData;

enum class ProjectionType : char
{
    UNSET = -1,
    ORTO = 0,
    PERSPECTIVE
};

#if _MSC_VER
GAME_ENGINE_TEMPLATE struct GAME_ENGINE_API glm::vec3;
class GAME_ENGINE_API CUL::GUTILS::SimpleDelegate;
#endif // #if _MSC_VER

class Camera final
{
public:
    GAME_ENGINE_API Camera();

    CUL::GUTILS::SimpleDelegate changeDelegate;

    glm::vec3 m_pos = { 0.f, 0.f, 0.f };
    glm::vec3 m_up = { 0.f, 1.f, 0.f };
    WindowSize m_size;
    float m_left = 0.0f;
    float m_right = 0.0f;
    float m_top = 0.0f;
    float m_bottom = 0.0f;

    void setProjectionType( const ProjectionType type );
    ProjectionType getProjectionType() const;

    GAME_ENGINE_API glm::mat4 getViewMatrix() const;
    GAME_ENGINE_API glm::mat4 getProjectionMatrix() const;

    float getLeft() const;
    float getRight() const;
    float getTop() const;
    float getBottom() const;

    GAME_ENGINE_API void setZNear( float zNear );
    GAME_ENGINE_API float getZnear() const;

    GAME_ENGINE_API void setZfar( float inZfar );
    GAME_ENGINE_API float getZfar() const;
    GAME_ENGINE_API float getFov() const;

    GAME_ENGINE_API void setAspectRatio( float value );
    GAME_ENGINE_API float getAspectRatio() const;

    GAME_ENGINE_API const glm::vec3& getCenter() const;

    GAME_ENGINE_API const glm::vec3& getEye() const;
    GAME_ENGINE_API const glm::vec3& getUp() const;

    GAME_ENGINE_API void setUp( const glm::vec3& pos );

    GAME_ENGINE_API void setFov( float val );
    GAME_ENGINE_API void setSize( const WindowSize& winSize );
    GAME_ENGINE_API void setEyePos( const glm::vec3& pos );
    GAME_ENGINE_API void setCenter( const glm::vec3& pos );
    GAME_ENGINE_API void toggleDepthTest( bool enable );
    GAME_ENGINE_API bool getDepthTestIsEnabled() const;
    GAME_ENGINE_API bool getProjectionChanged() const;
    GAME_ENGINE_API void toggleProjectionChanged( bool hasChanged );

    GAME_ENGINE_API ~Camera();

protected:
private:
    ProjectionType m_projectionType = ProjectionType::PERSPECTIVE;
    float m_aspectRatio = 16.f / 9.f;
    bool m_depthTest = true;
    float m_zNear = 64.f;
    float m_zFar = 100.f;
    float m_fov = 90.f;
    glm::vec3 m_target = { 0.f, 0.f, -255.f };
    glm::vec3 m_rightVec = { 0.f, 0.f, 0.f };
    std::atomic<bool> m_projectionDataChanged = false;

    Camera( const Camera& arg ) = delete;
    Camera( Camera&& arg ) = delete;
    Camera& operator=( const Camera& rhv ) = delete;
    Camera& operator=( Camera&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )