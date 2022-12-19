#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/IObject.hpp"

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
    const auto rot = getRotationAbsolute();
    //abs = local + parent
}

const CUL::MATH::Rotation TransformComponent::getParentRotation() const
{
    IObject* parent = m_owner.getParent();
    if( parent )
    {
    }
    else
    {
        return CUL::MATH::Rotation();
    }
}

const CUL::MATH::Rotation TransformComponent::getRotationAbsolute() const
{
    const glm::mat4 model = getModel();

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose( model, scale, rotation, translation, skew, perspective );

    CUL::MATH::Rotation result( rotation );

    return result;
}

const glm::mat4 TransformComponent::getModel() const
{
    glm::mat4 result( 1.f );

    const glm::vec3 translVec = getPositionToParent();
    result = glm::translate(result, translVec);

    glm::vec3 pivotReal = getPivotReal();
    glm::mat4 trans_to_pivot = glm::translate( glm::mat4( 1.0f ), pivotReal );
    glm::mat4 rotation = glm::toMat3( m_rotation.toQuat() );
    glm::mat4 trans_from_pivot = glm::translate( glm::mat4( 1.0f ), -pivotReal );

    glm::mat4 model = result * trans_to_pivot * rotation * trans_from_pivot;

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

TransformComponent::~TransformComponent()
{
}