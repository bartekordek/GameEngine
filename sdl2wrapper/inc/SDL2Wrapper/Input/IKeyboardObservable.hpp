#pragma once

#include "SDL2Wrapper/Input/KeyboardDefines.hpp"

#include "CUL/STL_IMPORTS/STD_functional.hpp"

NAMESPACE_BEGIN( SDL2W )

class IKeyboardObserver;

class SDL2WAPI IKeyboardObservable
{
public:
    IKeyboardObservable() = default;

    virtual void registerKeyboardEventCallback( const std::function<void( const KeyboardState& key )>& callback ) = 0;

    virtual void registerKeyboardEventListener( IKeyboardObserver* observer ) = 0;
    virtual void unregisterKeyboardEventListener( IKeyboardObserver* observer ) = 0;

    virtual bool isKeyUp( const CUL::String& keyName ) const = 0;

    virtual ~IKeyboardObservable() = default;
protected:
private:
    IKeyboardObservable( const IKeyboardObservable& arg ) = delete;
    IKeyboardObservable( IKeyboardObservable&& arg ) = delete;
    IKeyboardObservable& operator=( const IKeyboardObservable& arg ) = delete;
    IKeyboardObservable& operator=( IKeyboardObservable&& arg ) = delete;
};

NAMESPACE_END( SDL2W )