#include "gameengine/Camera.hpp"
#include "gameengine/ProjectionData.hpp"

using namespace LOGLW;

Camera::Camera( ProjectionData* projectionData )
    : m_pos( 0.0f, 0.0f, 0.0f ), m_center( 0.0f, 0.0f, 0.0f ), m_projectionData( projectionData )
{
}

glm::mat4 Camera::getViewMatrix()
{
    glm::vec3 front(0.0f, 0.0f, -1.0f);
    m_center = m_pos + front;
    glm::vec3 up(0.f, -1.f, 0.f);
    glm::mat4 viewMatrix = glm::lookAt( m_pos, m_center, up );
    return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix()
{
    float ar = m_projectionData->getAspectRatio();
    float zNear = m_projectionData->getZnear();
    float zFar = m_projectionData->getZfar();
    glm::mat4 projection = glm::perspective( glm::radians( m_fov ), ar, zNear, zFar );
    return projection;
}

Camera::~Camera()
{

}