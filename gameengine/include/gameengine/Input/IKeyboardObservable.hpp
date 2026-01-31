#pragma once

#include "gameengine/Input/KeyboardDefines.hpp"

#include "CUL/STL_IMPORTS/STD_functional.hpp"

NAMESPACE_BEGIN( LOGLW )

class IKeyboardObserver;

class GAME_ENGINE_API IKeyboardObservable
{
public:
    IKeyboardObservable() = default;

    virtual void registerKeyboardEventCallback( const std::function<void( const KeyboardState& key )>& callback ) = 0;

    virtual void registerKeyboardEventListener( IKeyboardObserver* observer ) = 0;
    virtual void unregisterKeyboardEventListener( IKeyboardObserver* observer ) = 0;

    virtual bool isKeyUp( const String& keyName ) const = 0;

    virtual ~IKeyboardObservable() = default;

protected:
private:
    IKeyboardObservable( const IKeyboardObservable& arg ) = delete;
    IKeyboardObservable( IKeyboardObservable&& arg ) = delete;
    IKeyboardObservable& operator=( const IKeyboardObservable& arg ) = delete;
    IKeyboardObservable& operator=( IKeyboardObservable&& arg ) = delete;
};

NAMESPACE_END( LOGLW )