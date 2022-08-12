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
    //IObject* parent = m_owner.getParent();
    //if(parent)
    //{
    //    TransformComponent* parentTransform = static_cast<TransformComponent*>( parent->getComponent( "TransformComponent" ) );
    //    if( parentTransform )
    //    {
    //        TransformComponent::Pos parentPos = parentTransform->getWorldPosition();
    //        return m_pos + parentPos;
    //    }
    //}

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
    bool old = true;
    if( old )
    {
        glm::vec3 pivotReal = getPivotReal();

        glm::mat4 trans_to_pivot = glm::translate( glm::mat4( 1.0f ), pivotReal );
        glm::mat4 trans_from_pivot = glm::translate( glm::mat4( 1.0f ), -pivotReal );

        glm::mat4 model = getTranslation() * trans_to_pivot * getRotation() * trans_from_pivot;

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
    else
    {
        glm::mat4 model = glm::mat4( 1.0f );

        //glm::vec3 pivot = m_pivot.toGlmVec();
        // glm::mat4 trans_to_pivot = glm::translate( glm::mat4( 1.0f ), -pivot );
        // glm::mat4 trans_from_pivot = glm::translate( glm::mat4( 1.0f ), pivot );
        // glm::mat4 rotation(1.f);
        //rotation =
        //glm::mat4 rotate = trans_from_pivot * rotate_matrix * trans_to_pivot;

        return model;
    }
}

glm::vec3 TransformComponent::getPivotReal()
{
    return m_pivot.toGlmVec() * m_size.toGlmVec();
}

glm::vec3 TransformComponent::getPivotNormalized()
{
    return m_pivot;
}

glm::mat4 TransformComponent::getRotation()
{
    glm::mat4 model(1.f);

    CUL::MATH::Rotation rotation = m_rotation;


    // Yaw
    {
        glm::vec3 normal( 0.f, 1.f, 0.f );
        model = glm::rotate( model, -rotation.yaw.getRad(), normal );
    }

    // Pitch
    {
        glm::vec3 normal( -1.f, 0.f, 0.f );
        model = glm::rotate( model, rotation.pitch.getRad(), normal );
    }

    // Roll
    {
        glm::vec3 normal( 0.f, 0.f, 1.f );
        model = glm::rotate( model, rotation.roll.getRad(), normal );
    }

    return model;
}

glm::mat4 TransformComponent::getTranslation()
{
    glm::mat4 result(1.f);

    const Pos& position = getWorldPosition();
    glm::vec3 posVec = position.toGlmVec() - getPivotReal();
    result = glm::translate( result, posVec );

    return result;
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

void TransformComponent::setSize( const Pos& size )
{
    m_size = size;
    changeSizeDelegate.execute();
}

const TransformComponent::Pos& TransformComponent::TransformComponent::getSize() const
{
    return m_size;
}

void TransformComponent::removeCallback(const String& callbackName)
{
    m_onChangeCallbacks.erase(callbackName);
}

TransformComponent::~TransformComponent()
{
}