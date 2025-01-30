#pragma once

#include "SDL2Wrapper/Import.hpp"

NAMESPACE_BEGIN( SDL2W )

class MouseData;

class SDL2WAPI IMouseObserver
{
public:
    IMouseObserver() = default;
    virtual void onMouseEvent( const MouseData& md ) = 0;

    virtual ~IMouseObserver() = default;
protected:
private:
    IMouseObserver( const IMouseObserver& arg ) = delete;
    IMouseObserver( IMouseObserver&& arg ) = delete;
    IMouseObserver& operator=( const IMouseObserver& arg ) = delete;
    IMouseObserver& operator=( IMouseObserver&& arg ) = delete;
};

NAMESPACE_END( SDL2W )