#pragma once

#include "SDL2Wrapper/Import.hpp"

NAMESPACE_BEGIN( SDL2W )

class IWindow;
class ISDL2Wrapper;
struct WindowData;

class SDL2WAPI IWindowFactory
{
public:
    IWindowFactory();

    virtual ~IWindowFactory();

    virtual IWindow* createWindow( const WindowData& winData, ISDL2Wrapper* wrapper ) = 0;

protected:
private:
    IWindowFactory( const IWindowFactory& rhv ) = delete;
    IWindowFactory( IWindowFactory&& rhv ) = delete;
    IWindowFactory& operator=( const IWindowFactory& rhv ) = delete;
    IWindowFactory& operator=( IWindowFactory&& rhv ) = delete;
};

NAMESPACE_END( SDL2W )