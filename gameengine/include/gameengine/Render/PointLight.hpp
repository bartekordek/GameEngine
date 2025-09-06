#pragma once

#include "gameengine/IObject.hpp"
#include "CUL/Graphics/Color.hpp"

namespace LOGLW
{
class TransformComponent;
class VertexArray;

class PointLight final: public IObject
{
public:
    GAME_ENGINE_API PointLight( IObject* parent );

    GAME_ENGINE_API void setColor( const CUL::Graphics::ColorS& color );

    GAME_ENGINE_API ~PointLight();

    PointLight( const PointLight& ) = delete;
    PointLight( PointLight&& ) = delete;
    PointLight& operator=( const PointLight& ) = delete;
    PointLight& operator=( PointLight&& ) = delete;

protected:
private:
    void init();
    void render() override;
    void setTransformation();
    void applyColor();
    
    TransformComponent* m_transformComponent{ nullptr };
    CUL::Graphics::ColorS m_color;
};
}  // namespace LOGLW