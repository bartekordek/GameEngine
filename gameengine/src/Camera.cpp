#include "gameengine/Camera.hpp"

using namespace LOGLW;

Camera::Camera()
{
}

glm::mat4 Camera::getViewMatrix()
{
    glm::mat4 viewMatrix = glm::lookAt( m_pos, m_target, m_up );
    return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix()
{
    glm::mat4 projection = glm::perspective( glm::radians( m_fov ), m_aspectRatio, m_zNear, m_zFar );
    return projection;
}

float Camera::getLeft() const
{
    return m_left;
}

float Camera::getRight() const
{
    return m_right;
}

float Camera::getTop() const
{
    return m_top;
}

float Camera::getBottom() const
{
    return m_bottom;
}

float Camera::getZnear() const
{
    return m_zNear;
}

float Camera::getZfar() const
{
    return m_zFar;
}

float Camera::getFov() const
{
    return m_fov;
}

float Camera::getAspectRatio() const
{
    return m_aspectRatio;
}

const glm::vec3& Camera::getCenter() const
{
    return m_center;
}

const glm::vec3& Camera::getEye() const
{
    return m_pos;
}

const glm::vec3& Camera::getUp() const
{
    return m_up;
}

void Camera::setUp( const glm::vec3& pos )
{
    m_up = pos;
}

void Camera::setZnear( float val )
{
    m_zNear = val;
}

void Camera::setZfar( float val )
{
    m_zFar = val;
}

void Camera::setFov( float val )
{
    m_fov = val;
}

void Camera::setSize( const WindowSize& winSize )
{
    m_size = winSize;

    m_left = m_center.x - 0.5f * (float)m_size.getWidth();
    m_right = m_center.x + 0.5f * (float)m_size.getWidth();

    m_bottom = m_center.y - 0.5f * (float)m_size.getHeight();
    m_top = m_center.y + 0.5f * (float)m_size.getHeight();
}

void Camera::setEyePos( const glm::vec3& pos )
{
    m_pos = pos;
}

void Camera::setCenter( const glm::vec3& pos )
{
    m_center = pos;
}

void Camera::toggleDepthTest( bool enable )
{
    m_depthTest = enable;
}

bool Camera::getDepthTestIsEnabled() const
{
    return m_depthTest;
}

Camera::~Camera()
{

}