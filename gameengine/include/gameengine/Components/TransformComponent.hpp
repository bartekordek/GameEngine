#pragma once

#include "gameengine/Components/IComponent.hpp"
#include "gameengine/Defines.hpp"

#include "CUL/Math/Point.hpp"
#include "CUL/Math/Rotation.hpp"
#include <CUL/String/StringWrapper.hpp>
#include "CUL/GenericUtils/SimpleDelegate.hpp"
#include "CUL/GenericUtils/DelegateTemplate.hpp"

#include "CUL/STL_IMPORTS/STD_functional.hpp"
#include "CUL/STL_IMPORTS/STD_map.hpp"

#include "CUL/IMPORT_GLM.hpp"

NAMESPACE_BEGIN( LOGLW )

using DPosChangeDelegate = CUL::GUTILS::DelegateTemplateThreeParam<float, float, float>;

class IObject;

class TransformComponent final: public IComponent
{
public:
    using Pos = CUL::MATH::Point;
    using TranslationVector = CUL::MATH::Point;

    GAME_ENGINE_API TransformComponent( IObject* owner );

    GAME_ENGINE_API void setPositionToParent( const glm::vec3& position );
    GAME_ENGINE_API const glm::vec3 getPositionToParent() const;

    GAME_ENGINE_API void setPositionAbsolute( const glm::vec3& position );
    GAME_ENGINE_API const glm::vec3 getPositionAbsolut() const;

    GAME_ENGINE_API void setRotationToParent( const CUL::MATH::Rotation& rotation );
    GAME_ENGINE_API const CUL::MATH::Rotation getRotationToParent() const;

    GAME_ENGINE_API void setRotationAbsolute( const CUL::MATH::Rotation& rotation );
    GAME_ENGINE_API const CUL::MATH::Rotation getRotationAbsolute() const;

    GAME_ENGINE_API const glm::mat4 getModel() const;

    GAME_ENGINE_API void setSize( const Pos& size );
    GAME_ENGINE_API const Pos& getSize() const;

    GAME_ENGINE_API const Pos& getPivot() const;
    GAME_ENGINE_API glm::vec3 getPivotNormalized();
    GAME_ENGINE_API void setPivot( const Pos& pivot );
    GAME_ENGINE_API void setPivotNormalized( const Pos& pivot );

    GAME_ENGINE_API void addOnChangeCallback( const String& callbackName, const std::function<void( const glm::mat4& model )> callback );
    GAME_ENGINE_API void removeCallback( const String& callbackName );

    CUL::GUTILS::SimpleDelegate changeSizeDelegate;
    DPosChangeDelegate PositionChangeDelegate;

    GAME_ENGINE_API void decomposeAndLogData( const glm::mat4& data ) const;
    GAME_ENGINE_API const glm::vec3& getScale() const;
    GAME_ENGINE_API void setScale( const glm::vec3& scale );
    GAME_ENGINE_API void move( const glm::vec3& inDiff );


    GAME_ENGINE_API ~TransformComponent();

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

    glm::vec3 m_size{ 0.f, 0.f, 0.f };
    glm::vec3 m_pos{ 0.f, 0.f, 0.f };
    glm::vec3 m_scale = { 1.f, 1.f, 1.f };
    CUL::MATH::Rotation m_rotation;
    glm::vec3 m_pivotNormalized{ 0.5f, 0.5f, 0.0f };
    glm::vec3 m_pivot{ 0.f, 0.f, 0.f };

    std::map<String, std::function<void( const glm::mat4 )> > m_onChangeCallbacks;

    // Deleted:
    TransformComponent( const TransformComponent& value ) = delete;
    TransformComponent( TransformComponent&& value ) = delete;
    TransformComponent& operator=( const TransformComponent& value ) = delete;
    TransformComponent& operator=( TransformComponent&& value ) = delete;
};

NAMESPACE_END( LOGLW )