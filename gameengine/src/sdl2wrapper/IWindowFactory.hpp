#pragma once

#include "gameengine/Import.hpp"

NAMESPACE_BEGIN( LOGLW )

class IWindow;
class ISDL2Wrapper;
class WinData;

class GAME_ENGINE_API IWindowFactory
{
public:
    IWindowFactory();

    virtual ~IWindowFactory();

    virtual IWindow* createWindow( const WinData& winData, ISDL2Wrapper* wrapper ) = 0;

protected:
private:
    IWindowFactory( const IWindowFactory& rhv ) = delete;
    IWindowFactory( IWindowFactory&& rhv ) = delete;
    IWindowFactory& operator=( const IWindowFactory& rhv ) = delete;
    IWindowFactory& operator=( IWindowFactory&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )