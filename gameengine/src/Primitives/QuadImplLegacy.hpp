#pragma once

#include "gameengine/IUtilityUser.hpp"
#include "gameengine/Primitives/IQuad.hpp"

NAMESPACE_BEGIN( LOGLW )

class QuadImplLegacy final:
    public IQuad,
    public IUtilityUser
{
public:
    QuadData m_data;

    QuadImplLegacy(IGameEngine* engine);

    void setColor( const QuadColors& colors ) override;
    void setColor( const ColorS& color ) override;

    ~QuadImplLegacy();
protected:
private:
    void setValues( const QuadData& values ) override;
    void render() override;

    QuadColors m_colors;

};

NAMESPACE_END( LOGLW )