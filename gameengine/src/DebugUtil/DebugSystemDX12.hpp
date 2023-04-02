#pragma once

#include "DebugUtil/DebugSystemBase.hpp"

NAMESPACE_BEGIN(LOGLW)

class DebugSystemDX12 final: public DebugSystemBase
{
public:
protected:
private:

	// Inherited via DebugSystemBase
	virtual void init( const DebugSystemParams& params ) override;
	virtual void frame() override;
	void addRenderCallback( const std::function<void( void )> renderDebugCallback ) override;
};

NAMESPACE_END(END)