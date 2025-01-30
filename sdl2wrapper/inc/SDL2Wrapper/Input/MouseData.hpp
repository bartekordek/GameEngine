#pragma once

#include "SDL2Wrapper/Import.hpp"

#include "CUL/ISerializable.hpp"
#include "CUL/GenericUtils/DumbPtr.hpp"
#include "CUL/STL_IMPORTS/STD_map.hpp"

NAMESPACE_BEGIN( SDL2W )

using MouseButtonIndex = unsigned short;

enum class WheelDirection: short
{
    NONE = 0,
    UP,
    DOWN
};

#if _MSC_VER
#pragma warning( push )
#pragma warning( disable: 4820 )
#endif
class SDL2WAPI MouseData: public CUL::ISerializable
{
public:
    enum class EventType: short
    {
        NONE = -1,
        MOTION = 0x400,
        MOUSEBUTTONDOWN,
        MOUSEBUTTONUP,
        MOUSEWHEEL
    };

    MouseData();
    MouseData( const MouseData& arg );
    MouseData( MouseData&& arg );

    MouseData& operator=( const MouseData& arg );
    MouseData& operator=( MouseData&& arg );

    unsigned int getMouseButtonCount() const;
    bool isButtonDown( const MouseButtonIndex buttonIndex ) const;
    int getX() const;
    int getY() const;
    int getWheelX() const;
    int getWheelY() const;
    WheelDirection getWheelDirection() const;
    EventType getEventType() const;

    void setPos( int x, int y );
    void setState( const MouseButtonIndex buttonIndex, const bool isUp );
    void setWheel( int x, int y, const WheelDirection direction );
    void setType( const EventType et );

    virtual ~MouseData();
protected:
private:
    CUL::String getSerializationContent( CUL::CounterType tabsSize, const bool = false ) const override;

    std::map<MouseButtonIndex, bool> m_buttonStates;
    int m_x = 0;
    int m_y = 0;
    int m_wheelX = 0;
    int m_wheelY = 0;
    WheelDirection m_wheelDirection = WheelDirection::NONE;
    EventType m_eventType = EventType::NONE;
};
#ifdef _MSC_VER
#pragma warning( pop )
#endif

NAMESPACE_END( SDL2W )