#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/IObject.hpp"

#include "CUL/GenericUtils/SimpleAssert.hpp"

using namespace LOGLW;

TransformComponent::TransformComponent(IObject& owner):m_owner(owner)
{
}

void TransformComponent::setWorldPosition( const Pos& position )
{
    m_pos = position;
}

void TransformComponent::setWorldPosition( Pos::Type x, Pos::Type y, Pos::Type z )
{
    m_pos[0] = x;
    m_pos[1] = y;
    m_pos[2] = z;
}

const TransformComponent::Pos TransformComponent::getWorldPosition() const
{
    IObject* parent = m_owner.getParent();
    if(parent)
    {
        TransformComponent* parentTransform = static_cast<TransformComponent*>( parent->getComponent( "TransformComponent" ) );
        if( parentTransform )
        {
            TransformComponent::Pos parentPos = parentTransform->getWorldPosition();
            return m_pos + parentPos;
        }
    }

    return m_pos;
}


void TransformComponent::setWorldAngle( CUL::MATH::EulerAngles type, const CUL::MATH::Angle& angle )
{
    if( type == CUL::MATH::EulerAngles::PITCH )
    {
        m_rotation.pitch = angle;
    }
    else if( type == CUL::MATH::EulerAngles::YAW )
    {
        m_rotation.yaw = angle;
    }
    else
    {
        m_rotation.roll = angle;
    }
}

void TransformComponent::setWorldRotation( const CUL::MATH::Rotation& rotation )
{
    m_rotation = rotation;
}

const CUL::MATH::Rotation TransformComponent::getWorldRotation() const
{
    if( true )
    {
        return m_rotation;
    }
    else
    {
        CUL::MATH::Rotation result = m_rotation;

        IObject* parent = m_owner.getParent();
        if( parent )
        {
            TransformComponent* parentTransform = static_cast<TransformComponent*>( parent->getComponent( "TransformComponent" ) );
            if( parentTransform )
            {
                const CUL::MATH::Rotation parentRotation = parentTransform->getWorldRotation();
                result += parentRotation;
            }
        }

        return result;
    }
}

const CUL::MATH::Angle& TransformComponent::getWorldAngle( CUL::MATH::EulerAngles type ) const
{
    if( type == CUL::MATH::EulerAngles::PITCH )
    {
        return m_rotation.pitch;
    }
    else if( type == CUL::MATH::EulerAngles::ROLL )
    {
        return m_rotation.roll;
    }
    else
    {
        return m_rotation.yaw;
    }
}

const glm::mat4 TransformComponent::getModel()
{
    glm::mat4 model = glm::mat4( 1.0f );

    const Pos& position = getWorldPosition();
    glm::vec3 posVec = position.toGlmVec();
    model = glm::translate( model, posVec );

    // CUL::MATH::Rotation rotation = getWorldRotation();
    CUL::MATH::Rotation rotation = getWorldRotation();
    rotation.yaw.setCurrentType( CUL::MATH::Angle::Type::RADIAN );
    rotation.pitch.setCurrentType( CUL::MATH::Angle::Type::RADIAN );
    rotation.roll.setCurrentType( CUL::MATH::Angle::Type::RADIAN );

    // Yaw
    {
        glm::vec3 normal( 0.f, 1.f, 0.f );
        model = glm::rotate( model, -rotation.yaw.getRad(), normal );
    }

    // Pitch
    {
        glm::vec3 normal( 1.f, 0.f, 0.f );
        model = glm::rotate( model, rotation.pitch.getRad(), normal );
    }

    // Roll
    {
        glm::vec3 normal( 0.f, 0.f, 1.f );
        model = glm::rotate( model, rotation.roll.getRad(), normal );
    }

    IObject* parent = m_owner.getParent();
    if( parent )
    {
        TransformComponent* parentTransform = static_cast<TransformComponent*>( parent->getComponent( "TransformComponent" ) );
        if( parentTransform )
        {
            return parentTransform->getModel() * model;
        }
    }


    return model;
}

const TransformComponent::Pos& TransformComponent::getPivot() const
{
    return m_pivot;
}

void TransformComponent::setPivot( const TransformComponent::Pos& pivot )
{
    m_pivot = pivot;
}

void TransformComponent::addOnChangeCallback(const String& callbackName, const std::function<void( const Pos& position )> callback)
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

void TransformComponent::removeCallback(const String& callbackName)
{
    m_onChangeCallbacks.erase(callbackName);
}

TransformComponent::~TransformComponent()
{
}