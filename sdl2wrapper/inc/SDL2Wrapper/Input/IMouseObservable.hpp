#pragma once

#include "SDL2Wrapper/Import.hpp"
#include "CUL/STL_IMPORTS/STD_functional.hpp"

NAMESPACE_BEGIN( SDL2W )

class MouseData;
class IMouseObserver;

class SDL2WAPI IMouseObservable
{
public:
    using MouseCallback = std::function<void( const MouseData& md )>;

    IMouseObservable() = default;

    virtual void addMouseEventCallback( const MouseCallback& callback ) = 0;

    virtual void registerMouseEventListener( IMouseObserver* observer ) = 0;
    virtual void unregisterMouseEventListener( IMouseObserver* observer ) = 0;

    virtual MouseData& getMouseData() = 0;

    virtual ~IMouseObservable() = default;

protected:
private:
    IMouseObservable( const IMouseObservable& arg ) = delete;
    IMouseObservable( IMouseObservable&& arg ) = delete;
    IMouseObservable& operator=( const IMouseObservable& arg ) = delete;
    IMouseObservable& operator=( IMouseObservable&& arg ) = delete;
};

NAMESPACE_END( SDL2W )