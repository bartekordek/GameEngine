#include "Primitives/QuadImpl.hpp"

using namespace LOGLW;

QuadImpl::QuadImpl()
{
    m_data[ 0 ] = m_data[ 1 ] = m_data[ 2 ] = m_data[ 3 ] = { 0.0f, 0.0f, 0.0f };
}

void QuadImpl::setValues( const QuadData& values )
{
    m_data = values;
}

void QuadImpl::render()
{
    //TODO:
    //getUtility()->matrixStackPush();
    //    getUtility()->translate( getWorldPosition() );
    //    static const auto type = CUL::MATH::Angle::Type::DEGREE;
    //    getUtility()->rotate( getWorldAngle( CUL::MATH::EulerAngles::YAW ).getValueF( type ),   0.f, 0.f, 1.f );
    //    getUtility()->rotate( getWorldAngle( CUL::MATH::EulerAngles::PITCH ).getValueF( type ), 0.f, 1.f, 0.f );
    //    getUtility()->rotate( getWorldAngle( CUL::MATH::EulerAngles::ROLL ).getValueF( type ),  1.f, 0.f, 0.f );

    //    getUtility()->setVertexArrayClientState( true );

    //    getUtility()->setVertexPointer( 3, LOGLW::DataType::FLOAT, 0, &m_data[0] );
    //    getUtility()->drawArrays( LOGLW::PrimitiveType::QUADS, 0, 4 );

    //    getUtility()->setVertexArrayClientState( false );
    //getUtility()->matrixStackPop();
}

void QuadImpl::setColor(const QuadColors& colors)
{
    m_colors = colors;
}

void QuadImpl::setColor(const ColorS& color)
{
    for( auto& m_color: m_colors )
    {
        m_color = color;
    }
}

QuadImpl::~QuadImpl()
{

}