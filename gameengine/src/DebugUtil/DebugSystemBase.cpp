#include "DebugUtil/DebugSystemBase.hpp"
#include "DebugUtil/DebugSystemDX12.hpp"
#include "DebugUtil/DebugSystemOpenGLModern.hpp"


using namespace LOGLW;

DebugSystemBase* DebugSystemBase::create( DebugSystem::RendererType type )
{
	if( type == DebugSystem::RendererType::DIRECTX_12 )
	{
		return new DebugSystemDX12();
	}
	else if( type == DebugSystem::RendererType::OPENGL_MODERN )
	{
		return new DebugSystemOpenGLModern();
	}

	return nullptr;
}