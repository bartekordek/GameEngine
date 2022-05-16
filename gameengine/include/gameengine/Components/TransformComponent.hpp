#pragma once

#include "gameengine/Components/IComponent.hpp"

#include "CUL/Math/Point.hpp"
#include "CUL/Math/Rotation.hpp"
#include "CUL/String.hpp"

#include "CUL/STL_IMPORTS/STD_functional.hpp"
#include "CUL/STL_IMPORTS/STD_map.hpp"

#include "CUL/IMPORT_GLM.hpp"

NAMESPACE_BEGIN( LOGLW )

class IObject;

using String = CUL::String;

class GAME_ENGINE_API TransformComponent final: public IComponent
{
public:
    using Pos = CUL::MATH::Point;
    using TranslationVector = CUL::MATH::Point;


    TransformComponent(IObject& owner);

    void setWorldPosition( const Pos& position );
    void setWorldPosition( Pos::Type x, Pos::Type y, Pos::Type z );
    const Pos getWorldPosition() const;


    void setWorldAngle( CUL::MATH::EulerAngles type, const CUL::MATH::Angle& angle );

    void setWorldRotation(const CUL::MATH::Rotation& rotation);
    const CUL::MATH::Rotation getWorldRotation() const;

    const CUL::MATH::Angle& getWorldAngle( CUL::MATH::EulerAngles type ) const;
    const glm::mat4 getModel();
    glm::mat4 getRotation();
    glm::mat4 getTranslation();

    void setSize( const Pos& size );
    const Pos& getSize() const;

    const Pos& getPivot() const;
    void setPivot( const Pos& pivot );

    void addOnChangeCallback( const String& callbackName, const std::function<void( const Pos& position )> callback );
    void removeCallback( const String& callbackName );

    ~TransformComponent();
protected:
private:
    IObject& m_owner;

    Pos m_size;
    Pos m_pos;
    Pos m_pivot = Pos(0.5f, 0.5f, 0.0f);

    CUL::MATH::Rotation m_rotation;

    std::map<String, std::function<void( const Pos& position )> > m_onChangeCallbacks;

    // Deleted:
    TransformComponent( const TransformComponent& value ) = delete;
    TransformComponent( TransformComponent&& value ) = delete;
    TransformComponent& operator=( const TransformComponent& value ) = delete;
    TransformComponent& operator=( TransformComponent&& value ) = delete;
};

NAMESPACE_END( LOGLW )