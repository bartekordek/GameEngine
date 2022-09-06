#include "Primitives/LineImpl.hpp"
#include "gameengine/Components/TransformComponent.hpp"

using namespace LOGLW;

LineImpl::LineImpl( IGameEngine* engine ) : ILine( engine )
{
    m_data[0] = m_data[1] = { 0.0f, 0.0f, 0.0f };

    m_transform = static_cast < TransformComponent*>( getComponent( "TransformComponent" ) );
}

void LineImpl::setValues( const LineData& values )
{
    m_data = values;
}

void LineImpl::render()
{
    getUtility()->matrixStackPush();
    auto position = m_transform->getWorldPosition();
    getUtility()->translate( position );
    static const auto type = CUL::MATH::Angle::Type::DEGREE;
    getUtility()->rotate( m_transform->getWorldAngle( CUL::MATH::EulerAngles::YAW ).getValueF( type ), 0.f, 0.f, 1.f );
    getUtility()->rotate( m_transform->getWorldAngle( CUL::MATH::EulerAngles::PITCH ).getValueF( type ), 0.f, 1.f, 0.f );
    getUtility()->rotate( m_transform->getWorldAngle( CUL::MATH::EulerAngles::ROLL ).getValueF( type ), 1.f, 0.f, 0.f );
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