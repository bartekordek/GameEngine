#include "Primitives/TriangleImpl.hpp"
#include "gameengine/Components/TransformComponent.hpp"

using namespace LOGLW;

TriangleImpl::TriangleImpl( IGameEngine* engine ) : ITriangle(engine)
{
}

void TriangleImpl::render()
{
    getUtility()->matrixStackPush();
    getUtility()->translate( getTransform()->getWorldPosition() );
    static const auto type = CUL::MATH::Angle::Type::DEGREE;
    getUtility()->rotate( getTransform()->getWorldAngle( CUL::MATH::EulerAngles::YAW ).getValueF( type ), 0.f, 0.f, 1.f );
    getUtility()->rotate( getTransform()->getWorldAngle( CUL::MATH::EulerAngles::PITCH ).getValueF( type ), 0.f, 1.f, 0.f );
    getUtility()->rotate( getTransform()->getWorldAngle( CUL::MATH::EulerAngles::ROLL ).getValueF( type ),  1.f, 0.f, 0.f );
    getUtility()->draw( m_values, m_colors );
    getUtility()->matrixStackPop();
}

void TriangleImpl::setValues( const TriangleData& values )
{
    m_values = values;
    m_triangle.setValues( values );
}

const std::vector<float> TriangleImpl::getVertices() const
{
    std::vector<float> result;
    return result;
}

void TriangleImpl::setColor( const TriangleColors& colors )
{
    m_colors = colors;
}

void TriangleImpl::setColor( const ColorS& colorInput )
{
    for( auto& color: m_colors )
    {
        color = colorInput;
    }
}

TriangleImpl::~TriangleImpl()
{
}