#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/IObject.hpp"

#include "CUL/Log/ILogger.hpp"
#include "CUL/Log/ILogContainer.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"

using namespace LOGLW;

TransformComponent::TransformComponent( IObject& owner ):m_owner( owner )
{
}

void TransformComponent::setPositionToParent( const glm::vec3& position )
{
    m_pos = position;
}

const glm::vec3 TransformComponent::getPositionToParent() const
{
    return m_pos;
}

void TransformComponent::setPositionAbsolute( const glm::vec3& position )
{
    const auto parent = m_owner.getParent();
    if( parent )
    {
        const auto parentAbsolutePos = parent->getTransform()->getPositionAbsolut();
        m_pos = position - parentAbsolutePos;
    }
    else
    {
        m_pos = position;
    }
}

const glm::vec3 TransformComponent::getPositionAbsolut() const
{
    const auto parent = m_owner.getParent();
    if( parent )
    {
        const auto parentAbsolutePos = parent->getTransform()->getPositionAbsolut();
        return parentAbsolutePos + m_pos;
    }
    else
    {
        return m_pos;
    }
}

void TransformComponent::setRotationToParent( const CUL::MATH::Rotation& rotation )
{
}

const CUL::MATH::Rotation TransformComponent::getRotationToParent() const
{
    return CUL::MATH::Rotation();
}

void TransformComponent::setRotationAbsolute( const CUL::MATH::Rotation& rotation )
{
    //rotation = absolute + delta;
    //delta = rotation - absolute;
    const auto rot = getRotationAbsolute();
    const auto delta = rotation - rot;
    m_rotation = delta;
}

const CUL::MATH::Rotation TransformComponent::getRotationAbsolute() const
{
    const auto parent = m_owner.getParent();
    if( parent )
    {
        const auto parentRotation = parent->getTransform()->getRotationAbsolute();
        return parentRotation + m_rotation;
    }
    else
    {
        return m_rotation;
    }
}

const glm::mat4 TransformComponent::getModel() const
{
    glm::vec3 pivotReal = getPivotReal();

    glm::mat4 trans_to_pivot = glm::translate( glm::mat4( 1.0f ), pivotReal );
    glm::mat4 trans_from_pivot = glm::translate( glm::mat4( 1.0f ), -pivotReal );

    glm::mat4 model = getTranslation() * trans_to_pivot * getRotation() * trans_from_pivot;

    IObject* parent = m_owner.getParent();
    if( parent )
    {
        TransformComponent* parentTransform = static_cast< TransformComponent* >( parent->getComponent( "TransformComponent" ) );
        if( parentTransform )
        {
            return parentTransform->getModel() * model;
        }
    }

    return model;
}

glm::mat4 TransformComponent::getTranslation() const
{
    glm::mat4 result( 1.f );
    result = glm::translate( result, m_pos - m_pivotReal.toGlmVec() );

    return result;
}

glm::mat4 TransformComponent::getRotation() const
{
    glm::mat4 result = glm::toMat4( m_rotation.toQuat() );
    return result;
}

glm::vec3 TransformComponent::getPivotReal() const
{
    return m_pivotReal.toGlmVec();
}

glm::vec3 TransformComponent::getPivotNormalized()
{
    return m_pivot;
}

const TransformComponent::Pos& TransformComponent::getPivot() const
{
    return m_pivot;
}

void TransformComponent::setPivot( const TransformComponent::Pos& pivot )
{
    m_pivot = pivot;
    m_pivotReal = m_size.toGlmVec() * m_pivot.toGlmVec();
}

void TransformComponent::addOnChangeCallback(const String& callbackName, const std::function<void( const Pos& position )> callback)
{
    auto it = m_onChangeCallbacks.find(callbackName);
    if( it == m_onChangeCallbacks.end() )
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
    m_pivotReal = m_size.toGlmVec() * m_pivot.toGlmVec();
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

void TransformComponent::decomposeAndLogData( const glm::mat4& data ) const
{
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose( data, scale, rotation, translation, skew, perspective );

    const auto rotationString = glm::to_string( rotation );
    const auto translationString = glm::to_string( translation );

    const auto name = m_owner.getName();

    CUL::LOG::LOG_CONTAINER::getLogger()->log( name + ", translation: " + translationString + ", rotation: " + rotationString );
}

TransformComponent::~TransformComponent()
{
}