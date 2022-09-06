#pragma once

#include "gameengine/IObject.hpp"
#include "gameengine/IUtility.hpp"
#include "gameengine/IUtilityUser.hpp"

NAMESPACE_BEGIN( LOGLW )

class TransformComponent;

class Line:
    public IObject,
    public IUtilityUser
{
public:
    GAME_ENGINE_API Line( IGameEngine* engine );

    GAME_ENGINE_API void setValues( const LineData& values );
    GAME_ENGINE_API void setColor( const LineColors& colors );
    GAME_ENGINE_API void setColor( const ColorS& color );

    GAME_ENGINE_API virtual ~Line() = default;
    GAME_ENGINE_API void render() override;

protected:
private:
    LineColors m_colors;
    TransformComponent* m_transform = nullptr;
    LineData m_data;

    Line( const Line& arg ) = delete;
    Line( Line&& arg ) = delete;
    Line& operator=( const Line& rhv ) = delete;
    Line& operator=( Line&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )