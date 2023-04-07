#include "DebugUtil/DebugSystemBase.hpp"
#include "DebugUtil/DebugSystemDX12.hpp"
#include "DebugUtil/DebugSystemOpenGLModern.hpp"


using namespace LOGLW;

DebugSystemBase* DebugSystemBase::create( SDL2W::RenderTypes::RendererType type )
{
    if( type == SDL2W::RenderTypes::RendererType::DIRECTX_12 )
	{
		return new DebugSystemDX12();
	}
    else if( type == SDL2W::RenderTypes::RendererType::OPENGL_MODERN )
	{
		return new DebugSystemOpenGLModern();
	}

	return nullptr;
}