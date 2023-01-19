#include "Primitives/QuadImplLegacy.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/IUtilityUser.hpp"

using namespace LOGLW;

QuadImplLegacy::QuadImplLegacy( IGameEngine* engine, bool forceLegacy ): IQuad( engine, forceLegacy )
{
    m_data[0] = m_data[1] = m_data[2] = m_data[3] = { 0.0f, 0.0f, 0.0f };
    m_transform = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
}

void QuadImplLegacy::setValues( const QuadData& values )
{
    m_data = values;
}

void QuadImplLegacy::render()
{
    getDevice()->matrixStackPush();
    getDevice()->translate( m_transform->getPositionAbsolut() );
    static const auto type = CUL::MATH::Angle::Type::DEGREE;
    const auto rotation = m_transform->getRotationAbsolute();
    getDevice()->rotate( rotation.Yaw.getValueF( type ), 0.f, 0.f, 1.f );
    getDevice()->rotate( rotation.Pitch.getValueF( type ), 0.f, 1.f, 0.f );
    getDevice()->rotate( rotation.Roll.getValueF( type ), 1.f, 0.f, 0.f );
    getDevice()->draw( m_data, m_colors );
    getDevice()->matrixStackPop();
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