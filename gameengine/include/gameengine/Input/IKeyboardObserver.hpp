#pragma once

#include "gameengine/Input/KeyboardDefines.hpp"

NAMESPACE_BEGIN( LOGLW )

class GAME_ENGINE_API IKeyboardObserver
{
public:
    IKeyboardObserver() = default;
    virtual ~IKeyboardObserver() = default;

    virtual void onKeyBoardEvent( const KeyboardState& key ) = 0;

protected:
private:
};

NAMESPACE_END( LOGLW )