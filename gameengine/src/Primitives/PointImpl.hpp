#pragma once

#include "gameengine/Primitives/IPoint.hpp"

NAMESPACE_BEGIN( LOGLW )

class PointImpl final:
    public IPoint,
    public IUtilityUser
{
public:
    PointImpl()
    {

    }

    inline void setColor( const ColorS& color ) override
    {
        m_color = color;
    }

    ~PointImpl()
    {

    }

protected:
private:

    void render() override
    {
        getUtility()->matrixStackPush();
        getUtility()->translate( m_pos );
        getUtility()->draw( getWorldPosition(), m_color );
        getUtility()->matrixStackPop();
    }

    ColorS m_color;
    Point m_pos; // Set to 0, 0, 0, as world position is exact object position.

};

NAMESPACE_END( LOGLW )