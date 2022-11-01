#pragma once

#include "gameengine/IRenderDevice.hpp"

NAMESPACE_BEGIN( LOGLW )

class DeviceDX9 final: public IRenderDevice
{
public:
    DeviceDX9( CUL::CULInterface* culInterface );
protected:
private:
    ContextInfo initContextVersion( SDL2W::IWindow* window ) override;

};

NAMESPACE_END( LOGLW )