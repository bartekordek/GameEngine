#pragma once

#include "gameengine/IRenderDevice.hpp"

NAMESPACE_BEGIN( LOGLW )

class DeviceOpenGL_Legacy final: public IRenderDevice
{
public:
    DeviceOpenGL_Legacy( CUL::CULInterface* culInterface);
protected:
private:
    void checkLastCommandForErrors();
    ContextInfo initContextVersion( SDL2W::IWindow* window ) override;
};

NAMESPACE_END( LOGLW )