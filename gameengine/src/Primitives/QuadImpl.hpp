#pragma once

#include "gameengine/Primitives/IQuad.hpp"

NAMESPACE_BEGIN( LOGLW )

class QuadImpl final:
    public IQuad,
    public IUtilityUser
{
public:
    QuadData m_data;

    QuadImpl(IGameEngine* engine);

    void setColor( const QuadColors& colors ) override;
    void setColor( const ColorS& color ) override;

    ~QuadImpl();
protected:
private:
    void setValues( const QuadData& values ) override;
    void render() override;

    QuadColors m_colors;

};

NAMESPACE_END( LOGLW )