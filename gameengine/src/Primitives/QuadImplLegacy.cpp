#include "Primitives/QuadImplLegacy.hpp"

using namespace LOGLW;

QuadImplLegacy::QuadImplLegacy()
{
    m_data[ 0 ] = m_data[ 1 ] = m_data[ 2 ] = m_data[ 3 ] = { 0.0f, 0.0f, 0.0f };
}

void QuadImplLegacy::setValues( const QuadData& values )
{
    m_data = values;
}

void QuadImplLegacy::render()
{
    getUtility()->matrixStackPush();
    getUtility()->translate( getWorldPosition() );
    static const auto type = CUL::MATH::Angle::Type::DEGREE;
    getUtility()->rotate( getWorldAngle( CUL::MATH::EulerAngles::YAW ).getValueF( type ),   0.f, 0.f, 1.f );
    getUtility()->rotate( getWorldAngle( CUL::MATH::EulerAngles::PITCH ).getValueF( type ), 0.f, 1.f, 0.f );
    getUtility()->rotate( getWorldAngle( CUL::MATH::EulerAngles::ROLL ).getValueF( type ),  1.f, 0.f, 0.f );
    getUtility()->draw( m_data, m_colors );
    getUtility()->matrixStackPop();
}

void QuadImplLegacy::setColor(const QuadColors& colors)
{
    m_colors = colors;
}

void QuadImplLegacy::setColor(const ColorS& color)
{
    for( auto& m_color: m_colors )
    {
        m_color = color;
    }
}

QuadImplLegacy::~QuadImplLegacy()
{

}