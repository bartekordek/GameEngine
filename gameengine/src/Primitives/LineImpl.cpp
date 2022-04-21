#include "Primitives/LineImpl.hpp"

using namespace LOGLW;

LineImpl::LineImpl()
{
    m_data[ 0 ] = m_data[ 1 ] = { 0.0f, 0.0f, 0.0f };
}

void LineImpl::setValues( const LineData& values )
{
    m_data = values;
}

void LineImpl::render()
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

void LineImpl::setColor(const LineColors& colors)
{
    m_colors = colors;
}

void LineImpl::setColor(const ColorS& color)
{
    for( auto& m_color: m_colors )
    {
        m_color = color;
    }
}

LineImpl::~LineImpl()
{

}