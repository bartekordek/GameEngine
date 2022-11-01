#include "DeviceDX9.hpp"

using namespace LOGLW;

DeviceDX9::DeviceDX9( CUL::CULInterface* culInterface ) : IRenderDevice( culInterface, false )
{
}

ContextInfo DeviceDX9::initContextVersion( SDL2W::IWindow* /*window*/ )
{
    ContextInfo result;
    return result;
}