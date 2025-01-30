#include "Sprite.hpp"
#include "SDL2Wrapper/IMPORT_SDL.hpp"
#include "TextureSDL.hpp"

using namespace SDL2W;

Sprite::Sprite()
{
}

CUL::Graphics::ITexture* Sprite::getTexture()
{
    return m_texture;
}

void Sprite::setTexture( CUL::Graphics::ITexture* inputTexture )
{
    m_texture = inputTexture;
    m_textureRealSize = inputTexture->getSize();
    m_textureRealSize.setZ( 1.0 );
    calculateSpriteAbsoluteSize();
}

CUL::Graphics::IObject::Type Sprite::getType() const
{
    return IObject::Type::SPRITE;
}

const Position3DDMutexed& Sprite::getPosition() const
{
    return position;
}

const Position3DDMutexed& Sprite::getRenderPosition() const
{
    return positionWithOffset;
}

const Position3DDMutexed& Sprite::getSizeReal() const
{
    return m_textureRealSize;
}

const Position3DDMutexed& Sprite::getSizeAbs() const
{
    return m_absoluteSize;
}

void Sprite::setPosition( const Position3DDMutexed& newPosition )
{
    position = newPosition;
    calculatePositionOffset();
}

void Sprite::setX( CDbl val )
{
    position.setX( val );
    calculatePositionOffset();
}

void Sprite::setY( CDbl val )
{
    position.setY( val );
    calculatePositionOffset();
}

void Sprite::setZ( CDbl val )
{
    position.setZ( val );
    calculatePositionOffset();
}

void Sprite::move( const Position3DDMutexed& moveVect )
{
    position += moveVect;
    calculatePositionOffset();
}

const Position3DDMutexed& Sprite::getScale() const
{
    return scale;
}

void Sprite::setScale( const CUL::MATH::Vector3Dd& scnewScale )
{
    scale = scnewScale;
    calculateSpriteAbsoluteSize();
}

void Sprite::calculatePositionOffset()
{
    positionWithOffset = position - m_pivotAbsolute;
}

void Sprite::rotate(
    const CUL::MATH::Angle& angle,
    CRT )
{
    yaw = angle;
}

const CUL::MATH::Angle& Sprite::getAngle(
    CRT rotationType ) const
{
    if( RT::YAW == rotationType )
    {
        return yaw;
    }
    return yaw; // TODO?
}

void Sprite::setPivot(
    const double px,
    const double py,
    const double pz,
    const PivotType type )
{
    if( PivotType::ABSOLUTE == type )
    {
        m_pivotAbsolute.setXYZ( px, py, pz );
        m_pivotNormalised = m_pivotAbsolute / m_textureRealSize;
    }
    else
    {
        m_pivotNormalised.setXYZ( px, py, pz );
        m_pivotAbsolute = m_textureRealSize * m_pivotNormalised;
    }
}

void Sprite::setPivotX( CDbl val, const PivotType type )
{
    if( PivotType::ABSOLUTE == type )
    {
        m_pivotAbsolute.setX( val );
        calculatePivotNormalised();
    }
    else
    {
        m_pivotNormalised.setX( val );
        calculatePivotAbsolute();
    }
}

void Sprite::setPivotY( CDbl val, const PivotType type )
{
    if( PivotType::ABSOLUTE == type )
    {
        m_pivotAbsolute.setY( val );
        calculatePivotNormalised();
    }
    else
    {
        m_pivotNormalised.setY( val );
        calculatePivotAbsolute();
    }
}

void Sprite::setPivotZ( CDbl val, const PivotType type )
{
    if( PivotType::ABSOLUTE == type )
    {
        m_pivotAbsolute.setZ( val );
        calculatePivotNormalised();
    }
    else
    {
        m_pivotNormalised.setZ( val );
        calculatePivotAbsolute();
    }
}

const Position3DDMutexed& Sprite::getPivot( const PivotType type ) const
{
    if( PivotType::ABSOLUTE == type )
    {
        return m_pivotAbsolute;
    }
    return m_pivotNormalised;
}

void Sprite::calculateSpriteAbsoluteSize()
{
    m_absoluteSize = m_textureRealSize * scale;
    calculatePivotAbsolute();
}

void Sprite::calculatePivotAbsolute()
{
    m_pivotAbsolute = m_pivotNormalised * m_absoluteSize;
}

void Sprite::calculatePivotNormalised()
{
    m_pivotNormalised = m_pivotAbsolute / m_absoluteSize;
}

Sprite::~Sprite()
{
}