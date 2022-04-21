#include "gameengine/Viewport.hpp"

using namespace LOGLW;
using String = CUL::String;

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
    return !(*this == arg);
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
    String tabs = getTab( tabsSize );

    String result;
    result = result + tabs + "    \"name\": \"Viewport\",\n";
    result = result + tabs + "    \"Position\": \n" + pos.serialize( tabsSize + 1, true );
    result = result + tabs + "    \"Size\": \n" + size.serialize( tabsSize + 1 );

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