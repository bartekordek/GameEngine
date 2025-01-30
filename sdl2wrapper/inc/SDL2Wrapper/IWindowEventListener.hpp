#pragma once

#include "SDL2Wrapper/WindowEventType.hpp"

NAMESPACE_BEGIN( SDL2W )

class SDL2WAPI IWindowEventObserver
{
public:
    IWindowEventObserver() = default;
    virtual ~IWindowEventObserver() = default;

    virtual void onWindowEvent( const WindowEvent::Type e ) = 0;

protected:
private:
private: // Deleted:
    IWindowEventObserver( const IWindowEventObserver& arg ) = delete;
    IWindowEventObserver( IWindowEventObserver&& arg ) = delete;
    IWindowEventObserver& operator=( const IWindowEventObserver& rhv ) = delete;
    IWindowEventObserver& operator=( IWindowEventObserver&& rhv ) = delete;
};

NAMESPACE_END( SDL2W )