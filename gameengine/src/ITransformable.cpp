#include "gameengine/ITransformable.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"

using namespace LOGLW;

ITransformable::ITransformable() : m_worldPos( 0.0f, 0.0f, 0.0f )
{
}

void ITransformable::setWorldPosition( const Pos& position )
{
    m_worldPos = position;
}

void ITransformable::setWorldPosition( Pos::Type x, Pos::Type y, Pos::Type z )
{
    m_worldPos[0] = x;
    m_worldPos[1] = y;
    m_worldPos[2] = z;
}

const ITransformable::Pos& ITransformable::getWorldPosition() const
{
    return m_worldPos;
}

void ITransformable::setWorldAngle( CUL::MATH::EulerAngles type, Pos::Type value )
{
    m_angles[(size_t)type].setValue( value, CUL::MATH::Angle::Type::RADIAN );
}

void ITransformable::setWorldAngle( CUL::MATH::EulerAngles type, const CUL::MATH::Angle& angle )
{
    m_angles[(size_t)type] = angle;
}

void ITransformable::setWorldRotation( const CUL::MATH::Rotation& rotation )
{
    m_angles[(size_t)CUL::MATH::EulerAngles::YAW] = rotation.yaw;
    m_angles[(size_t)CUL::MATH::EulerAngles::PITCH] = rotation.pitch;
    m_angles[(size_t)CUL::MATH::EulerAngles::ROLL] = rotation.roll;
}

const CUL::MATH::Rotation ITransformable::getWorldRotation() const
{
    CUL::MATH::Rotation result;

    result.yaw = m_angles[(size_t)CUL::MATH::EulerAngles::YAW];
    result.pitch = m_angles[(size_t)CUL::MATH::EulerAngles::PITCH];
    result.roll = m_angles[(size_t)CUL::MATH::EulerAngles::ROLL];

    return result;
}

float ITransformable::getWorldAngleF( CUL::MATH::EulerAngles type ) const
{
    return m_angles[(size_t)type].getValueF( CUL::MATH::Angle::Type::RADIAN );
}

const CUL::MATH::Angle& ITransformable::getWorldAngle( CUL::MATH::EulerAngles type ) const
{
    return m_angles[(size_t)type];
}

const ITransformable::Pos& ITransformable::getPivot() const
{
    return m_pivot;
}

void ITransformable::setPivot( const ITransformable::Pos& pivot )
{
    m_pivot = pivot;
}

void ITransformable::addOnChangeCallback(const String& callbackName, const std::function<void( const Pos& position )> callback)
{
    auto it = m_onChangeCallbacks.find(callbackName);
    if( it != m_onChangeCallbacks.end() )
    {
        m_onChangeCallbacks[callbackName] = callback;
    }
    else
    {
        CUL::Assert::simple(false, "Callback already places: " + callbackName);
    }
}

void ITransformable::removeCallback(const String& callbackName)
{
    m_onChangeCallbacks.erase(callbackName);
}

ITransformable::~ITransformable()
{
}