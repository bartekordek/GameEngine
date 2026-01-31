#pragma once

#include "gameengine/Components/IComponent.hpp"

#include "CUL/Math/Point.hpp"
#include "CUL/Math/Rotation.hpp"
#include "CUL/String/StringWrapper.hpp"
#include "CUL/GenericUtils/SimpleDelegate.hpp"

#include "CUL/STL_IMPORTS/STD_functional.hpp"
#include "CUL/STL_IMPORTS/STD_map.hpp"

#include "CUL/IMPORT_GLM.hpp"

NAMESPACE_BEGIN( LOGLW )

class IObject;

class GAME_ENGINE_API TransformComponent final: public IComponent
{
public:
    using Pos = CUL::MATH::Point;
    using TranslationVector = CUL::MATH::Point;

    TransformComponent( IObject* owner );

    void setPositionToParent( const glm::vec3& position );
    const glm::vec3 getPositionToParent() const;

    void setPositionAbsolute( const glm::vec3& position );
    const glm::vec3 getPositionAbsolut() const;

    void setRotationToParent( const CUL::MATH::Rotation& rotation );
    const CUL::MATH::Rotation getRotationToParent() const;

    void setRotationAbsolute( const CUL::MATH::Rotation& rotation );
    const CUL::MATH::Rotation getRotationAbsolute() const;

    const glm::mat4 getModel() const;

    void setSize( const Pos& size );
    const Pos& getSize() const;

    const Pos& getPivot() const;
    void setPivot( const Pos& pivot );

    glm::vec3 getPivotReal() const;
    glm::vec3 getPivotNormalized();

    void addOnChangeCallback( const CUL::StringWr& callbackName, const std::function<void( const glm::mat4& model )> callback );
    void removeCallback( const CUL::StringWr& callbackName );

    CUL::GUTILS::SimpleDelegate changeSizeDelegate;
    void decomposeAndLogData( const glm::mat4& data ) const;

    const glm::vec3& getScale() const;
    void setScale( const glm::vec3& scale );

    ~TransformComponent();

protected:
private:
    const CUL::StringWr& getName() const override;
    glm::mat4 getTranslation() const;
    glm::mat4 getRotation() const;
    void printCurrentState() const;

#if !CUL_SHIPPING_BUILD
    void drawDebug() override;
#endif  // !CUL_SHIPPING_BUILD

    IObject* m_owner{ nullptr };

    Pos m_size;
    glm::vec3 m_pos = { 0.f, 0.f, 0.f };
    glm::vec3 m_scale = { 1.f, 1.f, 1.f };
    CUL::MATH::Rotation m_rotation;
    glm::vec3 m_pivot{ 0.5f, 0.5f, 0.0f };
    glm::vec3 m_pivotReal{ 0.f, 0.f, 0.f };

    std::map<CUL::StringWr, std::function<void( const glm::mat4 )> > m_onChangeCallbacks;

    // Deleted:
    TransformComponent( const TransformComponent& value ) = delete;
    TransformComponent( TransformComponent&& value ) = delete;
    TransformComponent& operator=( const TransformComponent& value ) = delete;
    TransformComponent& operator=( TransformComponent&& value ) = delete;
};

NAMESPACE_END( LOGLW )