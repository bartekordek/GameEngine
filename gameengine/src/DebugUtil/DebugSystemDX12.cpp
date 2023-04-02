#include "DebugUtil/DebugSystemBase.hpp"
#include "DebugSystemDX12.hpp"

using namespace LOGLW;


DebugSystemBase::DebugSystemBase()
{
}

void DebugSystemBase::init( const DebugSystemParams& params )
{
}

void DebugSystemBase::frame()
{
}

DebugSystemBase::~DebugSystemBase()
{
}

void DebugSystemDX12::init( const DebugSystemParams& params )
{
}

void DebugSystemDX12::frame()
{
}

void DebugSystemDX12::addRenderCallback( const std::function<void( void )> renderDebugCallback )
{
}
