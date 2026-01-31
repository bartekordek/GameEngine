#include "gameengine/Viewport.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"

using namespace LOGLW;
using String = String;

Viewport::Viewport()
{
}

void Viewport::set( const Pos2Di& newPos, const Size2Di newSize )
{
    pos = newPos;
    size = newSize;
}

bool Viewport::operator!=( const Viewport& arg ) const
{
    return !( *this == arg );
}

bool Viewport::operator==( const Viewport& arg ) const
{
    if( this == &arg )
    {
        return true;
    }
    return pos == arg.pos && size == arg.size;
}

String Viewport::getSerializationContent( CUL::CounterType tabsSize, const bool ) const
{
    const String result =
        String::createFromPrintf( "Viewport:\nPos: %d, %d\nSize: %d, %d", pos.getX(), pos.getY(), size.getWidth(), size.getHeight() );
    return result;
}

Viewport& Viewport::operator=( const Viewport& rhv )
{
    if( this != &rhv )
    {
        pos = rhv.pos;
        size = rhv.size;
    }
    return *this;
}

Viewport::~Viewport()
{
}