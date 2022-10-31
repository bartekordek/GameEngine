#pragma once

#include "gameengine/Import.hpp"

NAMESPACE_BEGIN( LOGLW )

class IRenderDevice;


class GAME_ENGINE_API IUtilityUser
{
public:
    IUtilityUser();

    static void useUtility( IRenderDevice* utility );

    virtual ~IUtilityUser();
protected:
    static IRenderDevice* getDevice();

private:
    IUtilityUser( const IUtilityUser& arg ) = delete;
    IUtilityUser( IUtilityUser&& arg ) = delete;
    IUtilityUser& operator=( const IUtilityUser& arg ) = delete;
    IUtilityUser& operator=( IUtilityUser&& arg ) = delete;

    static IRenderDevice* s_utility;

};

NAMESPACE_END( LOGLW )