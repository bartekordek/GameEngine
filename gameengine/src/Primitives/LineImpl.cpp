#include "Primitives/LineImpl.hpp"
#include "gameengine/Components/TransformComponent.hpp"

using namespace LOGLW;

LineImpl::LineImpl( IGameEngine* engine ) : ILine( engine, false )
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
    getDevice()->matrixStackPush();
    auto position = m_transform->getWorldPosition();
    getDevice()->translate( position );
    static const auto type = CUL::MATH::Angle::Type::DEGREE;
    getDevice()->rotate( m_transform->getWorldAngle( CUL::MATH::EulerAngles::YAW ).getValueF( type ), 0.f, 0.f, 1.f );
    getDevice()->rotate( m_transform->getWorldAngle( CUL::MATH::EulerAngles::PITCH ).getValueF( type ), 0.f, 1.f, 0.f );
    getDevice()->rotate( m_transform->getWorldAngle( CUL::MATH::EulerAngles::ROLL ).getValueF( type ), 1.f, 0.f, 0.f );
    getDevice()->draw( m_data, m_colors );
    getDevice()->matrixStackPop();
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