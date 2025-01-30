#include "DebugUtil/DebugSystemDX12.hpp"
#include "DebugUtil/DebugSystemOpenGLModern.hpp"

using namespace LOGLW;

DebugSystemBase* DebugSystemBase::create( RenderTypes::RendererType type )
{
    if( type == RenderTypes::RendererType::DIRECTX_12 )
    {
        // return new DebugSystemDX12();
        return nullptr;
    }
    else if( type == RenderTypes::RendererType::OPENGL_MODERN )
    {
        return new DebugSystemOpenGLModern();
    }

    return nullptr;
}