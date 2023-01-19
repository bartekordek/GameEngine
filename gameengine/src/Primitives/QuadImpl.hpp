#pragma once

#include "gameengine/IUtilityUser.hpp"
#include "gameengine/Primitives/IQuad.hpp"

NAMESPACE_BEGIN( LOGLW )

class TransformComponent;
class IGameEngine;

class QuadImpl final:
    public IQuad,
    public IUtilityUser
{
public:
    QuadData m_data;

    QuadImpl( IGameEngine* engine, bool forceLegacy = false );

    void setColor( const QuadColors& colors ) override;
    void setColor( const ColorS& color ) override;

    ~QuadImpl();
protected:
private:
    void init();
    void setValues( const QuadData& values ) override;
    void render() override;

    IGameEngine& m_engine;
    class Camera* m_camera = nullptr;
    std::unique_ptr<class Program> m_shaderProgram;
    TransformComponent* m_transformComponent = nullptr;

    bool m_initialized = false;

    QuadColors m_colors;

    unsigned m_vao = 0;
    unsigned m_vbo = 0;

};

NAMESPACE_END( LOGLW )