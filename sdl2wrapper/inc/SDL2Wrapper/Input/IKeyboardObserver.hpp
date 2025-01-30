#pragma once

#include "SDL2Wrapper/Input/KeyboardDefines.hpp"

NAMESPACE_BEGIN( SDL2W )

class SDL2WAPI IKeyboardObserver
{
public:
    IKeyboardObserver() = default;
    virtual ~IKeyboardObserver() = default;

    virtual void onKeyBoardEvent( const KeyboardState& key ) = 0;

protected:
private:
};

NAMESPACE_END( SDL2W )