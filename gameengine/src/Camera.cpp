#include "gameengine/Camera.hpp"

using namespace LOGLW;

Camera::Camera()
{
    changeDelegate.addDelegate(
        [this]()
        {
            toggleProjectionChanged( true );
        } );
}

void Camera::setProjectionType( const ProjectionType type )
{
    m_projectionType = type;
    changeDelegate.execute();
}

glm::mat4 Camera::getViewMatrix()
{
    glm::mat4 viewMatrix = glm::lookAt( m_pos, m_target, m_up );
    return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix()
{
    glm::mat4 projection = glm::perspective( glm::radians( m_fov ), m_aspectRatio, m_zNear, m_target.z );
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

void Camera::setZNear( float zNear )
{
    m_zNear = zNear;
    changeDelegate.execute();
}

float Camera::getZnear() const
{
    return m_zNear;
}

void Camera::setZfar( float inZfar )
{
    m_zFar = inZfar;
    changeDelegate.execute();
}

float Camera::getZfar() const
{
    return m_zFar;
}

float Camera::getFov() const
{
    return m_fov;
}

void Camera::setAspectRatio( float value )
{
    m_aspectRatio = value;
    changeDelegate.execute();
}

float Camera::getAspectRatio() const
{
    return m_aspectRatio;
}

const glm::vec3& Camera::getCenter() const
{
    return m_target;
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
    changeDelegate.execute();
}

void Camera::setFov( float val )
{
    m_fov = val;
    changeDelegate.execute();
}

void Camera::setSize( const WindowSize& winSize )
{
    m_size = winSize;

    m_left = m_target.x - 0.5f * (float)m_size.w;
    m_right = m_target.x + 0.5f * (float)m_size.w;

    m_bottom = m_target.y - 0.5f * (float)m_size.h;
    m_top = m_target.y + 0.5f * (float)m_size.h;

    changeDelegate.execute();
}

void Camera::setEyePos( const glm::vec3& pos )
{
    m_pos = pos;
    changeDelegate.execute();
}

void Camera::setCenter( const glm::vec3& pos )
{
    m_target = pos;
    changeDelegate.execute();
}

void Camera::toggleDepthTest( bool enable )
{
    m_depthTest = enable;
    changeDelegate.execute();
}

bool Camera::getDepthTestIsEnabled() const
{
    return m_depthTest;
}

bool Camera::getProjectionChanged() const
{
    return m_projectionDataChanged;
}

void Camera::toggleProjectionChanged( bool hasChanged )
{
    m_projectionDataChanged = hasChanged;
}

ProjectionType Camera::getProjectionType() const
{
    return m_projectionType;
}

Camera::~Camera()
{
}