#include "SDL2Wrapper/Input/MouseData.hpp"

using namespace SDL2W;

MouseData::MouseData()
{
}

MouseData::MouseData( const MouseData& arg ):
	CUL::ISerializable(),
    m_buttonStates( arg.m_buttonStates ),
    m_x( arg.m_x ),
    m_y( arg.m_y ),
    m_wheelX( arg.m_wheelX ),
    m_wheelY( arg.m_wheelY ),
    m_wheelDirection( arg.m_wheelDirection )
{
}

MouseData::MouseData( MouseData&& arg ):
    m_buttonStates( std::move( arg.m_buttonStates ) ),
    m_x( arg.m_x ),
    m_y( arg.m_y ),
    m_wheelX( arg.m_wheelX ),
    m_wheelY( arg.m_wheelY ),
    m_wheelDirection( arg.m_wheelDirection )
{
}

MouseData& MouseData::operator=( const MouseData& arg )
{
    if( this != &arg )
    {
        m_buttonStates = arg.m_buttonStates;
        m_x = arg.m_x;
        m_y = arg.m_y;
        m_wheelX = arg.m_wheelX;
        m_wheelY = arg.m_wheelY;
        m_wheelDirection = arg.m_wheelDirection;
    }
    return *this;
}

MouseData& MouseData::operator=( MouseData&& arg )
{
    if( this != &arg )
    {
        m_buttonStates = std::move( arg.m_buttonStates );
        m_x = arg.m_x;
        m_y = arg.m_y;
        m_wheelX = arg.m_wheelX;
        m_wheelY = arg.m_wheelY;
        m_wheelDirection = arg.m_wheelDirection;
    }
    return *this;
}

unsigned int MouseData::getMouseButtonCount() const
{
    return static_cast<unsigned>(m_buttonStates.size());
}

#ifdef _MSC_VER
// Yes, I know that is a Spectre mitigation.
// But for now, I let this as TODO, since i don't know
// How to fix this.
// TODO
#pragma warning( push )
#pragma warning( disable: 5045 )
#endif
bool MouseData::isButtonDown( const MouseButtonIndex buttonIndex ) const
{
    if( m_buttonStates.find(buttonIndex) == m_buttonStates.end() )
    {
        return false;
    }
    else
    {
        return !m_buttonStates.at( buttonIndex );
    }
}
#ifdef _MSC_VER
#pragma warning( pop )
#endif

int MouseData::getX() const
{
    return m_x;
}

int MouseData::getY() const
{
    return m_y;
}

int MouseData::getWheelX() const
{
    return m_wheelX;
}

int MouseData::getWheelY() const
{
    return m_wheelY;
}

WheelDirection MouseData::getWheelDirection() const
{
    return m_wheelDirection;
}

MouseData::EventType MouseData::getEventType() const
{
    return m_eventType;
}

void MouseData::setPos( int x, int y )
{
    m_x = x;
    m_y = y;
}

void MouseData::setState( const MouseButtonIndex buttonIndex, const bool isUp )
{
    m_buttonStates[ buttonIndex ] = isUp;
}

void MouseData::setWheel( int x, int y, const WheelDirection direction )
{
    m_wheelX = x;
    m_wheelY = y;
    m_wheelDirection = direction;
}

void MouseData::setType( const EventType et )
{
    m_eventType = et;
}

MouseData::~MouseData()
{
}

CUL::String MouseData::getSerializationContent( CUL::CounterType tabsSize, const bool ) const
{
    CUL::String tabs = getTab( tabsSize );

    CUL::String result;
    result = result + tabs + "\"pos\":\n";
    result = result + tabs + "{\n";
    result = result + tabs + "    \"x\": " + CUL::String( m_x ) + ",\n";
    result = result + tabs + "    \"y\": " + CUL::String( m_y ) + "\n";
    result = result + tabs + "}\n";

    return result;
}
