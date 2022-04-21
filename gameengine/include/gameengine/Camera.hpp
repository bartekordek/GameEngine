#pragma once

#include "gameengine/Import.hpp"

#include "CUL/IMPORT_GLM.hpp"

NAMESPACE_BEGIN( LOGLW )

class ProjectionData;

class Camera final
{
public:
    Camera(ProjectionData* projectionData);

    float m_fov = 90.f;
    glm::vec3 m_pos;
    glm::vec3 m_center;

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();

    ~Camera();
protected:
private:
    ProjectionData* m_projectionData = nullptr;

    Camera( const Camera& arg ) = delete;
    Camera( Camera&& arg ) = delete;
    Camera& operator=( const Camera& rhv ) = delete;
    Camera& operator=( Camera&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )