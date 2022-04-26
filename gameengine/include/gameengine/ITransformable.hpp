#pragma once

#include "gameengine/IRenderable.hpp"

#include "CUL/Graphics/IPosition3DD.hpp"
#include "CUL/Math/Point.hpp"
#include "CUL/Math/Rotation.hpp"
#include "CUL/String.hpp"

#include "CUL/STL_IMPORTS/STD_array.hpp"
#include "CUL/STL_IMPORTS/STD_map.hpp"
#include "CUL/STL_IMPORTS/STD_functional.hpp"

NAMESPACE_BEGIN( LOGLW )

using String = CUL::String;

class GAME_ENGINE_API ITransformable
{
public:
    using Pos = CUL::MATH::Point;
    using TranslationVector = CUL::MATH::Point;

    ITransformable();

    void setWorldPosition( const Pos& position );
    void setWorldPosition( Pos::Type x, Pos::Type y, Pos::Type z );
    const Pos& getWorldPosition() const;

    void setWorldAngle( CUL::MATH::EulerAngles type, Pos::Type value );
    void setWorldAngle( CUL::MATH::EulerAngles type, const CUL::MATH::Angle& angle );

    void setWorldRotation(const CUL::MATH::Rotation& rotation);
    const CUL::MATH::Rotation getWorldRotation() const;

    float getWorldAngleF( CUL::MATH::EulerAngles type ) const;
    const CUL::MATH::Angle& getWorldAngle( CUL::MATH::EulerAngles type ) const;

    const Pos& getPivot() const;
    void setPivot( const Pos& pivot );

    void addOnChangeCallback( const String& callbackName, const std::function<void( const Pos& position )> callback );
    void removeCallback( const String& callbackName );

    virtual ~ITransformable();

protected:
private:
    Pos m_worldPos;
    Pos m_pivot;
    std::array<CUL::MATH::Angle, 3> m_angles;
    std::map<String, std::function<void( const Pos& position )> > m_onChangeCallbacks;

    // Deleted:
    ITransformable( const ITransformable& value ) = delete;
    ITransformable( ITransformable&& value ) = delete;
    ITransformable& operator=( const ITransformable& value ) = delete;
    ITransformable& operator=( ITransformable&& value ) = delete;
};

NAMESPACE_END( LOGLW )