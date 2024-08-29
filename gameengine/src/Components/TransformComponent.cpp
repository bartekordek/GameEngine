#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/IObject.hpp"

#include "CUL/Log/ILogger.hpp"
#include "CUL/Log/ILogContainer.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"

using namespace LOGLW;

TransformComponent::TransformComponent( IObject& owner ):m_owner( owner )
{
    changeSizeDelegate.addDelegate( [this]() {
        //printCurrentState();
    } );
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
    //return m_pos - m_pivotReal.toGlmVec();
    return m_pos;
}

void TransformComponent::setRotationToParent( const CUL::MATH::Rotation& rotation )
{
    m_rotation = rotation;
}

const CUL::MATH::Rotation TransformComponent::getRotationToParent() const
{
    return m_rotation;
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
    return m_rotation;
}

const glm::mat4 TransformComponent::getModel() const
{
    glm::vec3 pivotReal = getPivotReal();

    glm::mat4 trans_to_pivot = glm::translate( glm::mat4( 1.0f ), pivotReal );
    glm::mat4 trans_from_pivot = glm::translate( glm::mat4( 1.0f ), -pivotReal );

    glm::mat4 rotationMat = getRotation();

    glm::mat4 scale = glm::scale( glm::mat4( 1.f ), m_scale );

    glm::mat4 model = getTranslation();

    model = model * trans_to_pivot;
    model = model * rotationMat;
    model = model * trans_from_pivot;
    model = model * scale;

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
    auto pivotTimesScale = m_pivotReal.toGlmVec() * m_scale;
    result = glm::translate( result, m_pos - pivotTimesScale );

    return result;
}

glm::mat4 TransformComponent::getRotation() const
{
    glm::mat4 model( 1.f );

    const CUL::MATH::Rotation& rotation = m_rotation;
    // PITCH - X
// YAW - Y
// ROLL - Z


        // Pitch
    {
        glm::vec3 normal( 1.f, 0.f, 0.f );
        model = glm::rotate( model, rotation.Pitch.getRad(), normal );
    }
    // Yaw
    {
        glm::vec3 normal( 0.f, 1.f, 0.f );
        model = glm::rotate( model, -rotation.Yaw.getRad(), normal );
    }

    // Roll
    {
        glm::vec3 normal( 0.f, 0.f, 1.f );
        model = glm::rotate( model, rotation.Roll.getRad(), normal );
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

    changeSizeDelegate.execute();
}

void TransformComponent::addOnChangeCallback( const String& callbackName, const std::function<void( const glm::mat4& model )> callback )
{
    auto it = m_onChangeCallbacks.find( callbackName );
    if( it == m_onChangeCallbacks.end() )
    {
        m_onChangeCallbacks[callbackName] = callback;
    }
    else
    {
        CUL::Assert::simple( false, ( CUL::String( "Callback already places: " ) + callbackName ).cStr() );
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

const glm::vec3& TransformComponent::getScale() const
{
    return m_scale;
}

void TransformComponent::setScale( const glm::vec3& scale )
{
    m_scale = scale;
    changeSizeDelegate.execute();
}

TransformComponent::~TransformComponent()
{
}

const CUL::String ToString( const glm::vec3& val )
{
    CUL::String result;

    result = "( " + std::to_string( val.x ) + ", " + std::to_string( val.y ) + ", " + std::to_string( val.z ) + " )";

    return result;
}

void TransformComponent::printCurrentState() const
{
    CUL::LOG::LOG_CONTAINER::getLogger()->log( m_owner.getName() + ": " );
    CUL::LOG::LOG_CONTAINER::getLogger()->log( "Current Size: " + ToString( m_size.toGlmVec() ) );
    CUL::LOG::LOG_CONTAINER::getLogger()->log( "Current Position: " + ToString( m_pos ) );
    CUL::LOG::LOG_CONTAINER::getLogger()->log( "Current Scale: " + ToString( m_scale ) );
    CUL::LOG::LOG_CONTAINER::getLogger()->log( "Current Pivot Normalized: " + ToString( m_pivot.toGlmVec() ) );
    CUL::LOG::LOG_CONTAINER::getLogger()->log( "Current Pivot Real: " + ToString( m_pivotReal.toGlmVec() ) );
}
